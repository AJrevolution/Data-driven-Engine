#include "NetworkManager.h"

//packets to send data
ENetPacket* dataPacketObjectName;
ENetPacket* dataPacketTextureName;
ENetPacket* dataPacketSize;
ENetPacket* dataPacketPosition;
ENetPacket* dataPacketType;
ENetPacket* dataPacketDensity;
ENetPacket* dataPacketFriction;
ENetPacket* dataPacketFinSetup;

//Position handling
ENetPacket* dataPacketForcePosition;
ENetPacket* dataPacketLeft;
ENetPacket* dataPacketRight;
ENetPacket* dataPacketUp;

//Preamble to make networking work
ENetAddress address;
ENetHost* client;
ENetPeer* peer;
ENetEvent enetEvent;
int finSetupSender = 5;

int dataType; // prevents need to send over b2 data types
int iLevel = 0; //Handle level data being given
bool maxLevelCheck = false; //ensure incrementer doesn't go over maximum amount of values
//Online direction checks
int up = 1;
int left = 2;
int right = 3;
int* direction = new int;
b2Vec2* positionForced = new b2Vec2;

//Pointer data to overrite and make level data into objects of use
std::string* objectName = new std::string;
std::string* textureName = new std::string;
b2Vec2* size = new b2Vec2;
b2Vec2* position = new b2Vec2;
int* type = new int;
int* finSetupReceiver = new int;
float* density = new float;
float* friction = new float;

gameObject temp = gameObject(1, 1, b2_staticBody, b2Vec2(1, 1), b2Vec2(1, 3), "temp", "temp", 0);

NetworkManager::NetworkManager()
{
}


NetworkManager::~NetworkManager()
{
}

//Update function - works by getting player to send each button press to server, and then to other client - rather than brute forcing position changes
std::vector<gameObject*> NetworkManager::update(std::vector<gameObject*> gameObjects, b2World * world, std::vector<gameObject*>* events, bool* setup, bool* player2, bool* finSetup)
{
	//Send level to server -> then player 2 client
	if (gameObjects.size() > 0 || *setup == true)
	{
		if (events->size() > 0)
		{
			//CHECK FOR EVENTS
			for (int i = 0; i < events->size(); i++)
			{

				if (events->at(i)->events == SEND)
				{
					networkObjects = GameManager::networkSetup(networkObjects, player2);
					
					//Loop through all lua objects and send to server - which is sent to other player's client
					for (int i = 0; i < networkObjects.size(); i++)
					{
						//object name
						dataPacketObjectName = enet_packet_create(&networkObjects[i]->objectName, sizeof(networkObjects[i]->objectName), ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, dataPacketObjectName);
						enet_host_flush(client);
						//enet_packet_destroy(dataPacketObjectName);

						//texture name
						dataPacketTextureName = enet_packet_create(&networkObjects[i]->textureName, sizeof(networkObjects[i]->textureName), ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, dataPacketTextureName);
						//enet_host_flush(client);
						//enet_packet_destroy(dataPacketTextureName);

						//size
						dataPacketSize = enet_packet_create(&networkObjects[i]->phyData->size, sizeof(networkObjects[i]->phyData->size), ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, dataPacketSize);
						//enet_host_flush(client);
						//enet_packet_destroy(dataPacketSize);
						
						//physics position
						dataPacketPosition = enet_packet_create(&networkObjects[i]->phyData->position, sizeof(networkObjects[i]->phyData->position), ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, dataPacketPosition);
						//enet_host_flush(client);
						//enet_packet_destroy(dataPacketPosition);

						//box2d body type
						//Check what type of body it is to prevent sending of a box2d body
						if (networkObjects[i]->phyData->type == b2_dynamicBody)
						{
							dataType = 1;
						}
						else if (networkObjects[i]->phyData->type == b2_staticBody)
						{
							dataType = 2;
						}
						dataPacketType = enet_packet_create(&dataType, sizeof(dataType), ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, dataPacketType);
						//enet_host_flush(client);
						//enet_packet_destroy(dataPacketType);

						//density
						dataPacketDensity = enet_packet_create(&networkObjects[i]->phyData->density, sizeof(networkObjects[i]->phyData->density), ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, dataPacketDensity);
						//enet_host_flush(client);
						//enet_packet_destroy(dataPacketDensity);

						//friction
						dataPacketFriction = enet_packet_create(&networkObjects[i]->phyData->friction, sizeof(networkObjects[i]->phyData->friction), ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, dataPacketFriction);
						enet_host_flush(client);	
						//enet_packet_destroy(dataPacketFriction);
					}
					dataPacketFinSetup = enet_packet_create(&finSetupSender, sizeof(finSetupSender), ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(peer, 0, dataPacketFinSetup);
					//enet_host_flush(client);
					*finSetup = true;
					//enet_packet_destroy(dataPacketFinSetup);
				}

				else if (events->at(i)->events == ESCAPE)
				{
					if (peer != NULL)
					{
						enet_peer_disconnect_now(peer, 0);
					}
				}
			}
		}
	}

	//If game has finished setup the level for player 2
	//Check if player has moved to send data to server
	if (gameObjects.size() > 0 || *setup == false)
	{
		if (events->size() > 0)
		{
			for (int i = 0; i < events->size(); i++)
			{
				if (events->at(i)->events == NOTHING)
				{
					dataPacketForcePosition = enet_packet_create(&gameObjects[i]->phyData->position, sizeof(gameObjects[i]->phyData->position), ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(peer, 0, dataPacketForcePosition);
					enet_host_flush(client);
				}
				else if (events->at(i)->events == LEFT)
				{
					dataPacketLeft = enet_packet_create(&left, sizeof(left), ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(peer, 0, dataPacketLeft);
					enet_host_flush(client);
				}
				else if (events->at(i)->events == RIGHT)
				{
					dataPacketRight = enet_packet_create(&right, sizeof(right), ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(peer, 0, dataPacketRight);
					enet_host_flush(client);
				}
				else if (events->at(i)->events == UP)
				{
					dataPacketUp = enet_packet_create(&up, sizeof(up), ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(peer, 0, dataPacketUp);
					enet_host_flush(client);
				}
			}
		}
	}

	//Setup level by receiving data from server, hence different if statement
	if (*setup == true)
	{
		while (enet_host_service(client, &enetEvent, 1000) > 0)
		{
			switch (enetEvent.type)
			{
			case ENET_EVENT_TYPE_RECEIVE:
				printf("A packet of length %u containing %s was received from %s on channel %u.\n",
					enetEvent.packet->dataLength,
					enetEvent.packet->data,
					enetEvent.peer->data,
					enetEvent.channelID);

				if (*setup == true && *player2 == true)
				{
					//CLIENT 1 MUST BE THE USER THAT PRESSES RECEIVE, USER MUST PRESS RECEIVE THEN CLIENT 2 PRESS SEND TO SEND LEVEL DATA
					//memcpy(debugString, enetEvent.packet->data, enetEvent.packet->dataLength);

					if (iLevel == 0)
					{
						memcpy(finSetupReceiver, enetEvent.packet->data, enetEvent.packet->dataLength);
						if (*finSetupReceiver == 5)
						{
							*finSetup = true;
							std::cout << "flawless" << "\n\n";
							break;
						}
						memcpy(objectName, enetEvent.packet->data, enetEvent.packet->dataLength);
						maxLevelCheck = false;
					}
					else if (iLevel == 1)
					{
						memcpy(textureName, enetEvent.packet->data, enetEvent.packet->dataLength);
					}
					else if (iLevel == 2) { memcpy(size, enetEvent.packet->data, enetEvent.packet->dataLength); }
					else if (iLevel == 3) { memcpy(position, enetEvent.packet->data, enetEvent.packet->dataLength); }
					else if (iLevel == 4) { memcpy(type, enetEvent.packet->data, enetEvent.packet->dataLength); }
					else if (iLevel == 5) { memcpy(density, enetEvent.packet->data, enetEvent.packet->dataLength); }
					else if (iLevel == 6)
					{
						memcpy(friction, enetEvent.packet->data, enetEvent.packet->dataLength);
						if (iLevel == 6)
						{
							iLevel = 0;
							maxLevelCheck = true;
							gameObjects = GameManager::makeGameObject(gameObjects, networkObjects, *density, *friction, *type, *position, *size, *objectName, *textureName, 0);
							//debug to see if it is getting correct data
							float sizex = size->x;
							float sizey = size->y;
							float positionx = position->x;
							float positiony = position->y;
							std::cout << sizex << " (sizex), " << sizey << " (sizey), " << *objectName << " (object name), " << *type << " (type), " << *density << " (density), "
								<< positionx << " positionx, " << positiony << " positiony " << *friction << " friction\n";
						}
					}

					if (iLevel != 6 && maxLevelCheck == false)
					{
						iLevel++;
					}
				}
				if (*setup == false)
				{
					memcpy(direction, enetEvent.packet->data, enetEvent.packet->dataLength);
					if (*direction != 1 && *direction != 2 && *direction != 3)
					{
						std::cout << "I made it here to force position\n";
						memcpy(positionForced, enetEvent.packet->data, enetEvent.packet->dataLength);
						gameObjects = GameManager::forcePosition(gameObjects, *events, *positionForced, player2);

					}
					else
					{
						std::cout << "Get Direction with: " << *direction << std::endl;
						gameObjects = GameManager::getDirection(gameObjects, *events, *direction, player2);

					}
				}
			}
		}
	}

	//If network setup has been done, begin receiving data packets of player's button presses
	else
	{
		while (enet_host_service(client, &enetEvent, 10) > 0)
		{
			switch (enetEvent.type)
			{
			case ENET_EVENT_TYPE_RECEIVE:
				printf("A packet of length %u containing %s was received from %s on channel %u.\n",
					enetEvent.packet->dataLength,
					enetEvent.packet->data,
					enetEvent.peer->data,
					enetEvent.channelID);

				
				if (*setup == false)
				{
					memcpy(direction, enetEvent.packet->data, enetEvent.packet->dataLength);
					if (*direction != 1 && *direction != 2 && *direction != 3)
					{
						std::cout << "I made it here to force position\n";
						memcpy(positionForced, enetEvent.packet->data, enetEvent.packet->dataLength);
						gameObjects = GameManager::forcePosition(gameObjects, *events, *positionForced, player2);

					}
					else
					{
						std::cout << "Get Direction with: " << *direction << std::endl;
						gameObjects = GameManager::getDirection(gameObjects, *events, *direction, player2);

					}
				}
				break;
			}
		}
		
	}
	

	if (*player2 == false)
	{
		if (*setup == false)
		{
			networkObjects = gameObjects;
		}
		return networkObjects;
	}

	return gameObjects;
}

void NetworkManager::setup()
{
	if (enet_initialize() != 0)
	{
		std::cout << "Enet failed to initialise!" << "\n\n";
	}

	client = enet_host_create(NULL, 1, 2, 0, 0);
	if (client == NULL)
	{
		std::cout << "Client failed to initialise!" << "\n\n";
	}

	enet_address_set_host(&address, "localhost");
	address.port = 1234;
	peer = enet_host_connect(client, &address, 2, 0);

	if (peer == NULL)
	{
		std::cout << "No available peers for initializing an ENet connection.\n";
	}

	/* Wait up to 5 seconds for the connection attempt to succeed. */
	if (enet_host_service(client, &enetEvent, 5000) > 0 &&
		enetEvent.type == ENET_EVENT_TYPE_CONNECT)
	{
		puts("Connection to localhost:1234 succeeded.");
		enet_host_flush(client);
	}
	else
	{
		/* Either the 5 seconds are up or a disconnect event was */
		/* received. Reset the peer in the event the 5 seconds   */
		/* had run out without any significant event.            */
		enet_peer_reset(peer);
		puts("Connection to localhost:1234 failed.");
	}

}
/*enet_host_destroy(client);
atexit(enet_deinitialize);*/

/*	if (gameObjects[playerIncrementer]->phyData->body != NULL)
{
if (gameObjects[playerIncrementer]->phyData->body->GetLinearVelocity().y > gameObjects[playerIncrementer]->phyData->body->GetLinearVelocity().x)
{
speedNow = gameObjects[playerIncrementer]->phyData->body->GetLinearVelocity().y;
}
else
{
speedNow = gameObjects[playerIncrementer]->phyData->body->GetLinearVelocity().x;
}

if (speedNow < 0.50 && speedNow > 0.2)
{
gameObjects[playerIncrementer]->events = NOTHING;
events->push_back(gameObjects[playerIncrementer]);
}
}*/