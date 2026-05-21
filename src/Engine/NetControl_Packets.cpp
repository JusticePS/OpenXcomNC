#include "NetControl_Packets.h"
#include "Game.h"
#include "NetControl.h"
#include "../Geoscape/GeoscapeState.h"
#include "Options.h"
#include "../Savegame/Base.h"
#include "../Savegame/Craft.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Ufo.h"
#include "../Geoscape/ConfirmDestinationState.h"
#include "../Geoscape/UfoDetectedState.h"
#include "../Savegame/TwitchPilot.h"
#include "../Savegame/ResearchProject.h"
#include "../Mod/RuleResearch.h"
#include "../Engine/Language.h"
#include "Screen.h"
#include "Action.h"

namespace OpenXcom
{
namespace NetControlPackets
{
	void NetControl_PacketBase::Execute(uint8_t buffer[1024], int packetSize, NetControl* nc, Game* game)
	{
		//use switch statement I guess, and verify packet size
		if (packetSize >= sizeof(NetControl_PacketBase))
		{
			NetControl_PacketBase* packetBase = (NetControl_PacketBase*)(buffer);
			if (packetBase->sentinel[0] == 'X' && packetBase->sentinel[1] == 'C' && packetBase->sentinel[2] == 'O' && packetBase->sentinel[3] == 'M')
			{
				switch (packetBase->packetType)
				{
				case NetControlPacketTypes::INCREASE_GEOSCAPE_SPEED:
				case NetControlPacketTypes::DECREASE_GEOSCAPE_SPEED:
					if (packetSize >= sizeof(NetControl_ChangeGeoscapeSpeed))
					{
						NetControl_ChangeGeoscapeSpeed* changeSpeed = (NetControl_ChangeGeoscapeSpeed*)buffer;
						changeSpeed->_Execute(nc, game);
					}
					break;
				case NetControlPacketTypes::SETVOLUME:
					if (packetSize >= sizeof(NetControl_PacketVolume))
					{
						NetControl_PacketVolume* packVol = (NetControl_PacketVolume*)buffer;
						packVol->_Execute(nc, game);
					}
					break;
				case NetControlPacketTypes::ATTACK_UFO:
					if (packetSize >= sizeof(NetControl_AttackUfo))
					{
						NetControl_AttackUfo* attack = (NetControl_AttackUfo*)buffer;
						attack->_Execute(nc, game);
					}
					break;
				case NetControlPacketTypes::REQUEST_RESEARCH_INFO:
					if (packetSize >= sizeof(NetControl_RequestResearchInfo))
					{
						NetControl_RequestResearchInfo* research = (NetControl_RequestResearchInfo*)buffer;
						research->_Execute(nc, game);
					}
					break;
				case NetControlPacketTypes::RESEARCH_SET_SCIENTISTS:
					if (packetSize >= sizeof(NetControl_ResearchSetScientists))
					{
						NetControl_ResearchSetScientists* research = (NetControl_ResearchSetScientists*)buffer;
						research->_Execute(nc, game);
					}
					break;
				case NetControlPacketTypes::UI_PRESS_INTERACTIVE_SURFACE:
					if (packetSize >= sizeof(NetControl_UIPressInteractiveSurface))
					{
						NetControl_UIPressInteractiveSurface* press = (NetControl_UIPressInteractiveSurface*)buffer;
						press->_Execute(nc, game);
					}
					break;
				case NetControlPacketTypes::UI_RAW_TEXT:
					if (packetSize >= sizeof(NetControl_UIRawText))
					{
						NetControl_UIRawText* rawText = (NetControl_UIRawText*)buffer;
						rawText->_Execute(nc, game);
					}
					break;
				}
			}
		}
	}

	void NetControl_PacketVolume::_Execute(NetControl* nc, Game* game)
	{
		//note: 0 - 128
		if (sound == -1)
		{
			sound = Options::soundVolume;
		}
		if (music == -1)
		{
			music = Options::musicVolume;
		}
		if (ui == -1)
		{
			ui = Options::uiVolume;
		}
		game->setVolume(sound, music, ui);
	}

	void NetControl_ChangeGeoscapeSpeed::_Execute(NetControl* nc, Game* game)
	{
		if (game->isState(nc->geoscape))
		{
			if (packetType == NetControlPacketTypes::INCREASE_GEOSCAPE_SPEED)
			{
				nc->geoscape->increaseSpeed();
			}
			else
			{
				nc->geoscape->decreaseSpeed();
			}
		}
	}

	void NetControl_AttackUfo::_Execute(NetControl* nc, Game* game)
	{
		if (game->getSavedGame() != nullptr)
		{
			for (std::vector<Ufo*>::iterator i = game->getSavedGame()->getUfos()->begin(); i != game->getSavedGame()->getUfos()->end(); ++i)
			{
				if (!(*i)->getDetected())
					continue;
				if ((*i)->getId() == ufo_id)
				{
					auto* bases = game->getSavedGame()->getBases();
					for (auto iter = bases->begin(); iter != bases->end(); ++iter)
					{
						auto* crafts = (*iter)->getCrafts();
						if (crafts != nullptr)
						{
							for (auto craftIter = crafts->begin(); craftIter != crafts->end(); ++craftIter)
							{
								auto c = (*craftIter);
								if (c->getStatus() == "STR_READY" || ((c->getStatus() == "STR_OUT" || Options::craftLaunchAlways) && !c->getLowFuel() && !c->getMissionComplete()))
								{
									if (c->getStatus() == "STR_READY")
									{
										bool found = false;
										auto* twitchPilots = game->getSavedGame()->getTwitchPilots();
										if (pilot != 0 && pilot != -1)
										{
											if (twitchPilots != nullptr)
											{
												
												for (auto pilotIter = twitchPilots->begin(); pilotIter != twitchPilots->end(); ++pilotIter)
												{
													TwitchPilot* tp = *pilotIter;
													if (tp->matches(pilot))
													{
														found = true;
													}
												}
												if (!found)
												{
													TwitchPilot* newPilot = TwitchPilot::Create(pilot);
													twitchPilots->push_back(newPilot);
													found = true;
													//TODO: create new pilot, add to pilot list, set name, use that
													//TODO: Later in other class, send shoot down message with pilot ID
													//TODO: Also add kill count to pilot class
													//found = true;
												}
											}
										}
										if (found)
										{
											c->setTwitchPilot(pilot);
										}
									}

									CraftId craftId = c->getUniqueId();
									auto hash = std::hash<std::string>{}(std::string(craftId.first)) + craftId.second;
									if (hash == interceptor)
									{
										std::vector<Craft*> _crafts = {c};
										
										if (ConfirmDestinationState::checkStartingCondition(_crafts, *i).empty())
										{

											//this is the one!
											c->setDestination(*i);
											if (c->getRules()->canAutoPatrol())
											{
												// cancel auto-patrol
												c->setIsAutoPatrolling(false);
											}
											c->setStatus("STR_OUT");

											if (nc->detectionState != nullptr && game->isState(nc->detectionState))
											{
												nc->geoscape->timerReset();
												game->popState();
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	void NetControl_RequestResearchInfo::_Execute(NetControl* nc, Game* game)
	{
		NetControl_ResearchInfoLabs labInfoResponse;
		if (game->getSavedGame() != nullptr)
		{
			auto* bases = game->getSavedGame()->getBases();
			for (auto iter = bases->begin(); iter != bases->end(); ++iter)
			{
				if (*iter == nullptr)
					continue;
				auto& researchProjects = (*iter)->getResearch();
				if (researchProjects.size() == 0)
				{
					continue;
				}
				labInfoResponse.labSpaceAvailable = (*iter)->getFreeLaboratories();
				labInfoResponse.scientistsAvailable = (*iter)->getAvailableScientists();
				labInfoResponse.numProjects = researchProjects.size();

				nc->SendPacketBot(&labInfoResponse, sizeof(labInfoResponse));

				for (auto iter = researchProjects.begin(); iter != researchProjects.end(); ++iter)
				{
					NetControl_ResearchInfoProject projectInfoResponse;
					projectInfoResponse.scientistsAssigned = (*iter)->getAssigned();
					auto projectName = game->getLanguage()->getString((*iter)->getRules()->getName());
					auto progressStr = game->getLanguage()->getString((*iter)->getResearchProgress());
					strcpy_s(projectInfoResponse.name, projectName.c_str());
					strcpy_s(projectInfoResponse.progress, progressStr.c_str());

					nc->SendPacketBot(&projectInfoResponse, sizeof(projectInfoResponse));
				}
			}
		}
	}

	void NetControl_ResearchSetScientists::_Execute(NetControl* nc, Game* game)
	{
		NetControl_ResearchInfoLabs labInfoResponse;
		if (game->getSavedGame() != nullptr)
		{
			auto* bases = game->getSavedGame()->getBases();
			for (auto iter = bases->begin(); iter != bases->end(); ++iter)
			{
				if (*iter == nullptr)
					continue;
				auto* labBase = *iter;
				auto& researchProjects = (*iter)->getResearch();
				if (researchProjects.size() == 0)
				{
					continue;
				}

				for (auto iter = researchProjects.begin(); iter != researchProjects.end(); ++iter)
				{
					auto projectNameTrans = game->getLanguage()->getString((*iter)->getRules()->getName());
					std::string projectName = projectNameTrans.c_str();
					int searchNameLen = strnlen_s(name, 31);
					if (projectName.size() < searchNameLen)
						continue;
					int i;
					for (i = 0; i < searchNameLen; ++i)
					{
						if (tolower(projectName[i]) != tolower(name[i]))
						{
							break;
						}
					}
					if (i == searchNameLen)
					{
						bool outOfThingsToStealFrom = false;
						while (labBase->getAvailableScientists() < scientistsToAssign && !outOfThingsToStealFrom)
						{
							outOfThingsToStealFrom = true;
							for (int projectIndex = 0; projectIndex < researchProjects.size() && labBase->getAvailableScientists() < scientistsToAssign; ++projectIndex)
							{
								if (researchProjects[projectIndex] == *iter)
								{
									continue;
								}
								if (researchProjects[projectIndex]->getAssigned() <= 0)
								{
									continue;
								}

								researchProjects[projectIndex]->setAssigned(researchProjects[projectIndex]->getAssigned() - 1);
								labBase->setScientists(labBase->getScientists() + 1);
								outOfThingsToStealFrom = false;
							}
						}
						int numToAssign = std::min(scientistsToAssign, labBase->getAvailableScientists());
						(*iter)->setAssigned((*iter)->getAssigned() + numToAssign);
						labBase->setScientists(labBase->getScientists() - numToAssign);
					}
				}
			}
		}
	}

	void NetControl_UIPressInteractiveSurface::_Execute(NetControl* nc, Game* game)
	{
		
		State* state = game->peekState();
		if (state == nullptr)
		{
			return;
		}
		if (xpos >= 0 && ypos >= 0)
		{
			{
				SDL_Event simEv;
				simEv.type = SDL_MOUSEBUTTONDOWN;
				simEv.button.button = SDL_BUTTON_LEFT;
				simEv.button.x = xpos;
				simEv.button.y = ypos;
				Action action = Action(&simEv, game->getScreen()->getXScale(), game->getScreen()->getYScale(), game->getScreen()->getCursorTopBlackBand(), game->getScreen()->getCursorLeftBlackBand());
				SDL_WarpMouse(action.getLeftBlackBand() + action.getXMouse(), action.getTopBlackBand() + action.getYMouse() );

				state->handle(&action);
			}
			{
				SDL_Event simEv;
				simEv.type = SDL_MOUSEBUTTONUP;
				simEv.button.button = SDL_BUTTON_LEFT;
				simEv.button.x = xpos;
				simEv.button.y = ypos;
				Action action = Action(&simEv, game->getScreen()->getXScale(), game->getScreen()->getYScale(), game->getScreen()->getCursorTopBlackBand(), game->getScreen()->getCursorLeftBlackBand());
				state->handle(&action);
			}
			
			
		}
		else if (keyNumber != -1)
		{
			{
				SDL_Event simEv;
				simEv.type = SDL_KEYDOWN;
				simEv.key.keysym.sym = (SDLKey)keyNumber;
				Action action = Action(&simEv, game->getScreen()->getXScale(), game->getScreen()->getYScale(), game->getScreen()->getCursorTopBlackBand(), game->getScreen()->getCursorLeftBlackBand());
				state->handle(&action);
			}
			{
				SDL_Event simEv;
				simEv.type = SDL_KEYUP;
				simEv.key.keysym.sym = (SDLKey)keyNumber;
				Action action = Action(&simEv, game->getScreen()->getXScale(), game->getScreen()->getYScale(), game->getScreen()->getCursorTopBlackBand(), game->getScreen()->getCursorLeftBlackBand());
				state->handle(&action);
			}
		}
		else if (label[0] != 0)
		{
			std::string labelText = std::string(label);
			state->pressSurfaceWithLabel(labelText);
		}
	}

	void NetControl_UIRawText::_Execute(NetControl* nc, Game* game)
	{

		State* state = game->peekState();
		if (state == nullptr)
		{
			return;
		}
		std::string textStr = std::string(text);
		for (int i = 0; i < textStr.size(); ++i)
		{
			{
				SDL_Event simEv;
				simEv.type = SDL_KEYDOWN;
				simEv.key.keysym.sym = SDLKey::SDLK_UNKNOWN;
				simEv.key.keysym.unicode = textStr[i];
				Action action = Action(&simEv, game->getScreen()->getXScale(), game->getScreen()->getYScale(), game->getScreen()->getCursorTopBlackBand(), game->getScreen()->getCursorLeftBlackBand());
				state->handle(&action);
			}
			{
				SDL_Event simEv;
				simEv.type = SDL_KEYUP;
				simEv.key.keysym.sym = SDLKey::SDLK_UNKNOWN;
				simEv.key.keysym.unicode = textStr[i];
				Action action = Action(&simEv, game->getScreen()->getXScale(), game->getScreen()->getYScale(), game->getScreen()->getCursorTopBlackBand(), game->getScreen()->getCursorLeftBlackBand());
				state->handle(&action);
			}
		}
	}
}
}
