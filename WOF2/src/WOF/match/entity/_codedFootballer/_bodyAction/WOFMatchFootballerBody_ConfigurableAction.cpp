#include "WOFMatchFootballerBody_ConfigurableAction.h"

#include "float.h"

#include "../WOFMatchFootballer_Body.h"
#include "WOFMatchFootballerBody_ConfigurableActionDynamics_Scripted.h"

namespace WOF { namespace match {


int FootballerBody_ConfigurableAction::compareCommandTime(const void *_pSort1, const void *_pSort2) {

	Command* pSort1 = *((Command**) _pSort1);
	Command* pSort2 = *((Command**) _pSort2);

	float diff = (pSort1->mTime) - (pSort2->mTime);

	if (diff < 0.0f) 
		return -1;
	else if (diff > 0.0f) 
		return 1;

	return 0;
}

bool FootballerBody_ConfigurableAction::loadTime(BufferString& tempStr, DataChunk& chunk, Time& time, const Time& totalTime) {

	if (chunk.scanAttribute(tempStr, L"time", L"%f", &time)) {

		return true;
	}

	if (chunk.scanAttribute(tempStr, L"relTime", L"%f", &time)) {

		time *= totalTime;
	}

	return false;
}

FootballerBody_ConfigurableAction::Dynamics* FootballerBody_ConfigurableAction::findDynamics(const String& str) {

	StringHash hash = str.hash();

	for (DynamicsArray::Index i = 0; i < mDynamicsArray.size; ++i) {

		if (mDynamicsArray[i]->mNameHash == hash) {

			return mDynamicsArray[i];
		}
	}

	return NULL;
}

FootballerBody_ConfigurableAction::StockCommand_StartDynamics* FootballerBody_ConfigurableAction::findStartDynamicsCommand(const String& str) {

	StringHash hash = str.hash();

	for (Commands::Index i = 0; i < mCommands.count; ++i) {

		if (mCommands[i]->getType() == Command_StartDynamics) {

			StockCommand_StartDynamics& command = (StockCommand_StartDynamics&) mCommands[i];

			if (command.dynamics->mNameHash == hash)
				return &command;
		}
	}

	return NULL;
}

bool FootballerBody_ConfigurableAction::load(Footballer_Body& body, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv, const Time& animLength, const Time& tickLength, CommandProcessor* pCommandFactory) {

	mHashAdjustSpeedCommands = false;
	mNeededDynamicsStateCount = 0;
	DynamicsStates::Index activeDynamicsStateCount = 0;

	SoftRef<DataChunk> child = chunk.getFirstChild();

	while (child.isValid()) {

		if (child->equals(L"settings")) {

			mMinSpeedFactor = 0.0f;
			child->scanAttribute(tempStr, L"minSpeedFactor", L"%f", &mMinSpeedFactor);

			mMaxSpeedFactor = FLT_MAX;
			child->scanAttribute(tempStr, L"maxSpeedFactor", L"%f", &mMaxSpeedFactor);

		} else if (child->equals(L"dynamics")) {

			if (child->getAttribute(L"name", tempStr)) {

				StringHash nameHash = tempStr.hash();
				SoftPtr<Dynamics> dynamics;

				if (child->getAttribute(L"type", tempStr)) {

					if (tempStr.equals(L"scripted")) {

						WE_MMemNew(dynamics.ptrRef(), FootballerBody_ConfigurableActionDynamics_Scripted());

					} else {

						assrt(false);
					}

				} else {

					assrt(false);
					//WE_MMemNew(dynamics.ptrRef(), Dynamics());
				}

				if (dynamics.isValid()) {

					if (dynamics->load(body, tempStr, child, pConv)) {

						dynamics->mNameHash = nameHash;
						mDynamicsArray.addOne(dynamics);

					} else {

						WE_MMemDelete(dynamics.ptrRef());
						return false;
					}

				} else {

					return false;
				}

			} else {

				return false;
			}

		} else if (child->equals(L"commands")) {

			SoftRef<DataChunk> command = child->getFirstChild();

			while (command.isValid()) {

				if (command->equals(L"playSound")) {

					SoftPtr<StockCommand_PlaySound> cmd;

					WE_MMemNew(cmd.ptrRef(), StockCommand_PlaySound());

					cmd->mTime = 0.0f;
					loadTime(tempStr, command, cmd->mTime, animLength);

					mCommands.addOne(cmd);

				} else if (command->equals(L"startDynamics")) {

					if (command->getAttribute(L"name", tempStr)) {

						SoftPtr<Dynamics> dynamics = findDynamics(tempStr);

						if (dynamics.isValid()) {

							SoftPtr<StockCommand_StartDynamics> cmd;
						
							WE_MMemNew(cmd.ptrRef(), StockCommand_StartDynamics());

							cmd->dynamics = dynamics;
							cmd->mTime = 0.0f;
							loadTime(tempStr, command, cmd->mTime, animLength);

							command->scanAttribute(tempStr, L"adjustSpeedFactor", cmd->adjustSpeedFactor);

							if (cmd->adjustSpeedFactor)
								mHashAdjustSpeedCommands = true;

							if (command->getAttribute(L"linkPrev", tempStr)) {

								if (tempStr.equals(L"body")) {

									cmd->linkPrevBody = true;

								} else {

									cmd->linkPrev = findDynamics(tempStr);

									if (!cmd->linkPrev.isValid()) {

										return false;
									}
								}
							}

							mCommands.addOne(cmd);
							cmd->dynamicsStateIndex = activeDynamicsStateCount;
							++activeDynamicsStateCount;

							mNeededDynamicsStateCount = MMax(mNeededDynamicsStateCount, activeDynamicsStateCount);

						} else {

							return false;
						}

					} else {

						return false;
					}

				} else if (command->equals(L"stopDynamics")) {

					if (command->getAttribute(L"name", tempStr)) {

						SoftPtr<StockCommand_StartDynamics> startCommand = findStartDynamicsCommand(tempStr);

						if (startCommand.isValid()) {

							SoftPtr<StockCommand_StopDynamics> cmd;
						
							WE_MMemNew(cmd.ptrRef(), StockCommand_StopDynamics());


							cmd->startCommand = startCommand;
							cmd->mTime = 0.0f;
							loadTime(tempStr, command, cmd->mTime, animLength);

							mCommands.addOne(cmd);
							--activeDynamicsStateCount;

						} else {

							return false;
						}

					} else {

						return false;
					}

				} else {

					if (pCommandFactory) {

						SoftPtr<Command> cmd = pCommandFactory->createCommand(tempStr, command, pConv, animLength);

						if (cmd->mTime < 0.0f) {

							cmd->mTime = 0.0f;
							loadTime(tempStr, command, cmd->mTime, animLength);
						}

						if (cmd.isValid()) {

							mCommands.addOne(cmd);

						} else {

							return false;
						}

					} else {

						return false;
					}
				}

				toNextSibling(command);
			}
		}

		toNextSibling(child);
	}

	qsort(mCommands.el, mCommands.count, sizeof(Command*), compareCommandTime);

	return true;
}

float FootballerBody_ConfigurableAction::getSpeedFactor(Footballer_Body& body, const Vector3& bodyAcc, const Vector3& bodyVel, const float& defaultSpeedFactor, const Time& tickLength, DynamicsStates& dynamicsStates) {

	DynamicsStates::Index activeDynamicsStateCount = 0;

	if (mHashAdjustSpeedCommands) {

		float maxRatio = -1.0f;

		Vector3 inOutAcc;
		Vector3 inOutVel;

		for (Commands::Index i = 0; i < mCommands.count; ++i) {

			if (mCommands[i]->getType() == Command_StopDynamics) {

				StockCommand_StopDynamics& record = dref((StockCommand_StopDynamics*) mCommands[i]);

				if (record.startCommand->linkPrevBody) {

					inOutAcc = bodyAcc;
					inOutVel = bodyVel;

				} else if (record.startCommand->linkPrev.isValid()) {

					inOutAcc = &dynamicsStates[record.startCommand->dynamicsStateIndex].acc;
					inOutVel = &dynamicsStates[record.startCommand->dynamicsStateIndex].vel;

				} else {

					inOutAcc = &Vector3::kZero;
					inOutVel = &Vector3::kZero;
				}

				Time t = 0;

				if (record.startCommand->dynamics->isSimulable(body) 
					&& record.startCommand->dynamics->hasRestingState(body, inOutAcc, inOutVel)
					&& record.startCommand->dynamics->simulate(body, tickLength, outAcc, outVel, t)) {

				} else {

					assrt(false);
				}

				dynamicsStates[record.startCommand->dynamicsStateIndex].acc = outAcc;
				dynamicsStates[record.startCommand->dynamicsStateIndex].vel = outVel;

				float ratio = t / (record->endTime - record->startTime);

				maxRatio = MMax(ratio, maxRatio);
			}
		}

		if (maxRatio <= 0.0f)
			assrt(false);

		float speedFactor = 1.0f / maxRatio;

		if (speedFactor < mMinSpeedFactor)
			speedFactor = mMinSpeedFactor;

		if (speedFactor > mMaxSpeedFactor)
			speedFactor = mMaxSpeedFactor;

		return speedFactor;
	}

	return defaultSpeedFactor;
}

} }