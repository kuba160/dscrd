#include <stdio.h>
#include <pthread.h>
#include <time.h>


#include "../dscrd.h"
#include "../dscrd_i.h"

#include "websocket.h"

// time to wait in each sleep (µs)
#define SLEEP_TIME 100000

// timeout in ms
#define CONNECT_TIMEOUT 3000

struct message_callback_info {
	const char *name;
	int (*func)(GatewayResp *);
	void *userdata;
};

typedef struct DiscordGateway_s {
	/// reserved
	char _name[21];
	//
	uint8_t _type;

	DiscordClient *discord_client;

	bool connection_estabilished;
	bool identified;
	int heartbeat_interval;

	client_websocket_t *websocket;

	pthread_t std_thread;

	bool std_abort;

	pthread_t heartbeat_thread;
	bool heartbeat_abort;

	int sequence_number;

	client_websocket_callbacks_t *std_callbacks;

	bool lazyload_guilds;
	DiscordGuild **lz_guilds;
	int lz_count;
	int lz_array_size;

	struct message_callback_info *msg_cbks[32];
} DiscordGateway;


typedef struct DiscordPayload_s {
	// opcode
	int op;
	int s;
	char *t;
	// data
	void *d;
	int d_type;

	//
	int heartbeat_interval;
} DiscordPayload;


DiscordPayload * DiscordPayload_read (char *json_data) {
	json_error_t error;
	json_t *root = json_loads(json_data, 0, &error);
	if (!root) {
		dscrd_error_set ("Failed to parse payload: error on line %d: %s",error.line, error.text);
		return NULL;
	}

	DiscordPayload *temp = malloc (sizeof(DiscordPayload));
	if (!temp) {
		json_decref (root);
		return NULL;
	}
	memset (temp, 0, sizeof(DiscordPayload));

	json_t *data;
	data = json_object_get(root, "op");
	if (data && json_is_integer(data)) {
		temp->op = json_integer_value(data);
	}
	data = json_object_get(root, "s");
	if (data && json_is_integer(data)) {
		temp->s = json_integer_value(data);
	}
	data = json_object_get(root, "t");
	if (data && json_is_string(data)) {
		temp->t = strdup(json_string_value(data));
	}

	data = json_object_get(root, "d");
	if (temp->op == 10) {
		if (temp->t == NULL) {
			json_t *sec;
			sec = json_object_get (data, "heartbeat_interval");
			if (sec && json_is_integer(sec)) {
				temp->heartbeat_interval = json_integer_value(sec);
			}
		}
	}
	else if (temp->op == 0 && strcmp(temp->t, "GUILD_CREATE") == 0) {
		char *s = json_dumps(data, 0);
		if (s) {
			temp->d = JTS(DiscordGuild, s);
			temp->d_type = DSCRD_GUILD;
			free (s);
		}
	}
	else if (temp->op == 0 && strcmp(temp->t, "MESSAGE_CREATE") == 0) {
		char *s = json_dumps(data, 0);
		if (s) {
			temp->d = JTS_EX(DiscordMessage, s);
			temp->d_type = DSCRD_MESSAGE;
			free (s);
		}
	}
	// todo more opcodes

	json_decref (root);


	return temp;
}

void DiscordPayload_free (DiscordPayload *payload) {
	if (payload->d && payload->d_type) {
		switch(payload->d_type) {
			case DSCRD_GUILD:
				JTS_FREE(DiscordGuild, payload->d);
				break;
			case DSCRD_MESSAGE:
				JTS_EX_FREE(DiscordMessage, payload->d);
				break;
			default:
				printf ("DiscordPayload_free: unknown type\n");
		}

	}

	if (payload->t) {
		free (payload->t);
	}
	free (payload);
}

int gateway_identify(DiscordGateway *gateway) {
	char *json_data = NULL;
	{
		json_t *root = json_object();
		// op
		json_object_set_new (root, "op", json_integer(2));

		// d
		json_t *data_object = NULL;
		json_t *properties_object = NULL;
		{
			data_object = json_object();
			json_object_set_new (data_object, "token", json_string(gateway->discord_client->token));

			// properties
			properties_object = json_object();
			{
				char *os_string = "unknown";
				#ifdef _WIN32
				os_string = "windows";
				#elif defined(__APPLE__)
				os_string = "mac os x";
				#elif defined(unix) || defined(__unix__) || defined(__unix)
				os_string = "linux";
				#endif
				json_object_set_new (properties_object, "$os", json_string(os_string));
				json_object_set_new (properties_object, "$browser", json_string("dscrd"));
				json_object_set_new (properties_object, "$device", json_string("dscrd"));
			}
			json_object_set_new (data_object, "properties",properties_object);
		}

		json_object_set_new (data_object, "compress", json_false());
		json_object_set_new (data_object, "large_threshold", json_integer(250));

		// shard

		// presence
		json_t *presence_object = NULL;
		{
			presence_object = json_object();
			// since
			// game
			// status
			json_object_set_new (presence_object, "status", json_string("online"));
			// afk
			json_object_set_new (presence_object, "afk", json_false());
		}
		json_object_set_new (data_object, "presence", presence_object);
		
		json_object_set_new (root, "d", data_object);

		json_data = json_dumps (root, 0);
		json_decref (root);
	}

	if (json_data) {
		websocket_send(gateway->websocket, json_data, strlen(json_data), 0);
		free (json_data);
	}
	else {
		dscrd_error_set ("Failed to generate json data.\n");
		return 1;
	}
	return 0;
}




void *std_thread (void *userdata) {
	DiscordGateway *data = (DiscordGateway *) userdata;
	printf ("std_thread start\n");
	while (1) {
		if (data->std_abort) {
			break;
		}
		websocket_think(data->websocket);
	}
	return NULL;
}


void *heartbeat_thread (void *userdata) {
	printf ("heartbeat_thread start\n");
	DiscordGateway *data = (DiscordGateway *) userdata;
	while (1) {
		char packet[128];
		sprintf(packet, "{\"op\": 1, \"d\": %i}", data->sequence_number);
		websocket_send(data->websocket, packet, strlen(packet), 0);

		int sleep_time = 0;
		while (sleep_time < 41250*1000) {
			usleep(41250*10);
			sleep_time += 41250*10;
			if (data->heartbeat_abort) {
				break;
			}
		}
		if (data->heartbeat_abort) {
			break;
		}
	}
	return NULL;
}


int dscrd_gateway_callback(client_websocket_t* socket, char* data, size_t length) {
	DiscordGateway *gateway = (DiscordGateway *) websocket_get_userdata(socket);
	DiscordPayload *payload = DiscordPayload_read (data);
	if (payload->op == 11) {
		// Heartbeat ACK
	}
	else {
		//printf ("dscrd_gateway_callback: data %s\n",data);
	}
	if (payload) {
		printf ("Received payload with opcode %d.\n", payload->op);
		if (payload->op == 11) {
			// Heartbeat ACK

		}
		else if (payload->op == 10) {
			// how to get DiscordGateway? :(
			gateway->connection_estabilished = true;
			// heartbeat thread?
			pthread_create(&gateway->heartbeat_thread, NULL, heartbeat_thread, (void*)gateway);


		}
		else if (payload->op == 9) {
			printf ("Invalid session error.\n");
			dscrd_error_set ("Invalid session.");
			DiscordPayload_free (payload);
			return -1;
		}
		else if (payload->op == 0) {
			if (strcmp(payload->t, "READY") == 0) {
				gateway->identified = true;
			}
			else if (gateway->lazyload_guilds && strcmp(payload->t, "GUILD_CREATE") == 0) {
				printf ("GUILD_CREATE : %s\n", data);
				if (gateway->lz_count == 0) {
					// new list
					gateway->lz_guilds = malloc (sizeof(DiscordGuild *) * 8);
					if (gateway->lz_guilds) {
						gateway->lz_array_size = 8;
					}
				}
				// append to list
				// steal it
				if (payload->d) {
					gateway->lz_guilds[gateway->lz_count] = (DiscordGuild *) payload->d;
					gateway->lz_guilds[gateway->lz_count]->discord_client = gateway->discord_client;
					payload->d = 0;
					dscrd_client_process (gateway->discord_client, DSCRD_GUILD, CREATE);
					gateway->lz_count++;
				}
				else {
					printf ("Failed to read payload data for GUILD_CREATE\n");
				}
				if (gateway->lz_count >= gateway->lz_array_size) {
					// double the size
					gateway->lz_guilds = realloc (gateway->lz_guilds, sizeof(DiscordGuild *) * gateway->lz_array_size * 2);
					if (gateway->lz_guilds) {
						gateway->lz_array_size = gateway->lz_array_size * 2;
					}
				}
			}
			else if (strcmp(payload->t, "MESSAGE_CREATE") == 0) {
				// left for callback
			}
			else {
				printf ("PAYLOAD NOT PROCESSED TODO\n");
				printf ("dscrd_gateway_callback: data %s\n",data);
			}
		}
		else {
			printf ("PAYLOAD NOT PROCESSED TODO\n");
		}

		// custom callbacks
		if (payload->op == 0) {
			int ci;
			GatewayResp rr;
			memset (&rr, 0, sizeof(GatewayResp));
			rr.event = payload->t;
			rr.data = payload->d;
			rr.data_type = payload->d_type;
			struct message_callback_info **cb_arr = gateway->msg_cbks;
			for (ci = 0; cb_arr[ci]; ci++) {
				if (strcmp(cb_arr[ci]->name, payload->t) == 0) {
					rr.userdata = cb_arr[ci]->userdata;
					int callback_ret = cb_arr[ci]->func(&rr);
					if (callback_ret == CALLBACK_ABORT) {
						DiscordGateway_callback_remove(gateway, ci);
					}
				}
			}
		}

		DiscordPayload_free (payload);
	}
	else {
		fprintf (stderr, "Failed to read payload. Raw data:\n\"%s\"\n",data);
	}

	return 0;
}

int dscrd_gateway_error_callback(client_websocket_t* socket, char* data, size_t length) {
	printf ("dscrd_gateway_error_callback: data %s\n",data);
	return 0;
}

void DiscordGateway_close (DiscordClient *client) {
	DiscordGateway *gateway = (DiscordGateway *) client->gateway;

	gateway->connection_estabilished = 0;
	gateway->identified = false;

	gateway->std_abort = 1;
	gateway->heartbeat_abort = 1;

	if (gateway->std_thread)
		pthread_join(gateway->std_thread, NULL);
	if (gateway->heartbeat_thread)
		pthread_join(gateway->heartbeat_thread, NULL);

	websocket_free (gateway->websocket);

	if (gateway->lz_guilds) {
		int i;
		for (i = 0; i < gateway->lz_count; i++) {
			JTS_FREE(DiscordGuild, gateway->lz_guilds[i]);
			dscrd_client_process (gateway->discord_client, DSCRD_GUILD, DESTROY);
		}
		free (gateway->lz_guilds);
		gateway->lz_guilds = NULL;
	}

	int i;
	for (i = 0; gateway->msg_cbks[i]; i++) {
		GatewayResp resp = {0};
		resp.terminate = 1;
		resp.userdata = gateway->msg_cbks[i]->userdata;
		gateway->msg_cbks[i]->func (&resp);
		free (gateway->msg_cbks[i]);
	}

	free (gateway);
}


void * DiscordGateway_open (DiscordClient *client) {
	DiscordGateway *gateway = malloc (sizeof(DiscordGateway));
	if (!gateway) {
		dscrd_error_set ("Failed to allocate memory.");
		return NULL;
	}
	else {
		memset (gateway, 0, sizeof(DiscordGateway));
		gateway->discord_client = client;
		//_type = DSCRD_GATEWAY;
	}

	gateway->std_callbacks = malloc(sizeof(client_websocket_callbacks_t));
	
	gateway->std_callbacks->on_receive = dscrd_gateway_callback;
	gateway->std_callbacks->on_connection_error = dscrd_gateway_error_callback;

	gateway->websocket = websocket_create(gateway->std_callbacks);
	if (!gateway->websocket) {
		DiscordGateway_close(client);
		return NULL;
	}
	if (client->gateway_flags) {
		if (client->gateway_flags == LAZYLOAD_GUILDS) {
			gateway->lazyload_guilds = true;
		}
	}
	websocket_set_userdata(gateway->websocket, gateway);
	//gateway->
	// TODO version selection
	websocket_connect(gateway->websocket, "wss://gateway.discord.gg/?v=6&encoding=json");

	
	// Offload websocket_think() to another thread so we can use mutex locks!
	//pthread_t serviceThread;
	//pthread_create(&serviceThread, NULL, thinkFunction, (void*)myWebSocket);
	// As we won't be joining the thread, we can safefully detach (which avoids memory leaks)
	//pthread_detach(serviceThread);

	client->gateway = gateway;

	pthread_create(&gateway->std_thread, NULL, std_thread, (void*)gateway);


	int time_slept = 0;
	while (!gateway->connection_estabilished) {
		usleep (SLEEP_TIME);
		time_slept += SLEEP_TIME;
		if (time_slept >= CONNECT_TIMEOUT * 1000) {
			break;
		}
	}
	if (!gateway->connection_estabilished) {
		// failed to connect
		// todo better error
		dscrd_error_set ("Failed to estabilish connection.");
		DiscordGateway_close (client);
		// std_callbacks
		return NULL;

	}


	// identify
	gateway_identify (gateway);

	while (!gateway->identified) {
		usleep (SLEEP_TIME);
		time_slept += SLEEP_TIME;
		if (time_slept >= CONNECT_TIMEOUT * 1000) {
			break;
		}
	}
	if (!gateway->identified) {
		dscrd_error_set ("Could not identify to the gateway.");
		DiscordGateway_close (client);
	}

	return gateway;
}

void DiscordGateway_send (DiscordClient *client, const char *message) {
	DiscordGateway *gateway = (DiscordGateway *) client->gateway;
	websocket_send ((client_websocket_t *) gateway->websocket, (char *) message, strlen(message), 0);
	// todo investigate need of char *
}


DiscordGuild ** DiscordClient_lazy_guilds (DiscordClient *client) {
	if (!client->gateway) {
		dscrd_error_set ("Client is not connected to Discord Gateway.");
		return NULL;
	}
	DiscordGateway *gateway = (DiscordGateway *) client->gateway;
	if (!gateway->lazyload_guilds) {
		dscrd_error_set ("Guilds were not loaded, add LAZYLOAD_GUILDS to your gateway flags.");
		return NULL;
	}
	// wait until we get guilds
	int time_slept = 0;
	while (!gateway->lz_guilds) {
		usleep (SLEEP_TIME);
		time_slept += SLEEP_TIME;
		if (time_slept >= CONNECT_TIMEOUT * 1000 * 2) {
			break;
		}
	}

	if (!gateway->lz_guilds) {
		return NULL;
	}

	// NULL terminate
	gateway->lz_guilds[gateway->lz_count] = NULL;

	return gateway->lz_guilds;
}


void DiscordGateway_callback_remove (void *gateway2, int i) {
	DiscordGateway *gateway = (DiscordGateway *) gateway2;
	{
		GatewayResp resp = {0};
		resp.terminate = 1;
		gateway->msg_cbks[i]->func (&resp);
	}
	free (gateway->msg_cbks[i]);
	memmove (gateway->msg_cbks + (i), gateway->msg_cbks + (i+1), sizeof(struct message_callback_info *) * (32 - i));
}


int DiscordGateway_callback (DiscordClient *client, char *event, int (*func)(GatewayResp *), void *userdata) {
	DiscordGateway *gateway = (DiscordGateway *) client->gateway;
	int i;
	for (i = 0; gateway->msg_cbks[i]; i++);

	if (i >= 31) {
		dscrd_error_set ("Max count of callbacks.");
		return -1;
	}

	struct message_callback_info *temp = malloc (sizeof(struct message_callback_info));

	if (!temp) {
		dscrd_error_set ("Memory.");
		return -2;
	}


	temp->name = event;
	temp->func = func;
	temp->userdata = userdata;

	gateway->msg_cbks[i++] = temp;
	gateway->msg_cbks[i] = NULL;

	return 0;
}