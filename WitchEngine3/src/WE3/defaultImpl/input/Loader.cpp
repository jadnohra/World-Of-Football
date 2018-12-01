#include "Loader.h"

#include "../../binding/win/includeWindows.h"

#include "../../input/Controller.h"
#include "../../input/Controller_Virtual.h"
#include "../../input/Effect_Constant.h"
#include "../../input/Effect_And.h"
#include "../../input/Effect_2SourcesToAxis.h"
#include "../../input/Effect_Negate.h"

#include "float.h"

namespace WE { namespace Input {

bool Loader::acceptSource(SourceHolder& source, float filterMin, float filterMax, bool checkMarked) {

	if (!source.isValid())
		return false;

	if (checkMarked && source->isMarked())
		return false;
	

	if (filterMin != -FLT_MAX) {

		SoftPtr<Component_Generic1D> comp = (Component_Generic1D*) source->getComponent(ComponentID_Generic1D);

		if (!comp.isValid() || !(comp->getMin() == filterMin)) {

			return false;
		}
	}

	if (filterMax != FLT_MAX) {

		SoftPtr<Component_Generic1D> comp = (Component_Generic1D*) source->getComponent(ComponentID_Generic1D);

		if (!comp.isValid() || !(comp->getMax() == filterMax)) {

			return false;
		}
	}

	return true;
}

bool Loader::getSource(Controller& controller, String* pAddress, DataChunk* pChunk, bool allowDummy, float filterMin, float filterMax, bool checkMarked, Controller* pMarkController, SourceHolder& source) {

	source.destroy();

	if (pAddress != NULL) {

		if (pAddress->length() > 0 && pAddress->c_tstr()[0] == L'@') {

			unsigned int sourceIndex;

			if (pAddress->fromString(L"@%u", &sourceIndex) && sourceIndex < controller.getSourceCount()) {
							
				controller.getSourceAt(sourceIndex, source, 0.0f);
			}

		} else {

			controller.getSource(pAddress->c_tstr(), source, 0.0f);

			if (!source.isValid()) {

				unsigned int sourceIndex;

				if (pAddress->fromString(L"%u", &sourceIndex) && sourceIndex < controller.getSourceCount()) {
							
					controller.getSourceAt(sourceIndex, source, 0.0f);
				}
			}
		}

		if (!acceptSource(source, filterMin, filterMax, checkMarked)) {

			source.destroy();
		}

	} else {

		SoftRef<DataChunk> preferChunk;
		BufferString tempStr;

		if (pChunk != NULL) {

			preferChunk = pChunk->getChild(L"prefer");
		}

		Controller::Index sourceCount = controller.getSourceCount();
		
		while (preferChunk.isValid()) {

			if (preferChunk->getAttribute(L"type", tempStr)) {

				for (Controller::Index i = 0; i < sourceCount; ++i) {

					if (controller.getSourceAt(i, source, 0.0f)) {

						if (tempStr.equalsIgnoreCase(source->getType()) 
							&& acceptSource(source, filterMin, filterMax, checkMarked))
							break;
					}

					source.destroy();
				}
			}

			if (source.isValid())
				break;

			preferChunk = preferChunk->getSibling(L"prefer");
		}

		if(!source.isValid()) {

			for (Controller::Index i = 0; i < sourceCount; ++i) {

				if (controller.getSourceAt(i, source, 0.0f)) {

					if (acceptSource(source, filterMin, filterMax, checkMarked))
						break;
				}

				source.destroy();
			}
		}
	}

	if (source.isValid() && pMarkController != NULL) {

		source->mark(pMarkController);
	}

	if (!source.isValid()) {

		if (!allowDummy || !Effect_Constant::create(0.0f, L"dummy", source)) {
			
			return false;
		}
	} 

	return true;
}

void Loader::processInputProfile(Input::InputManager& manager, DataChunk& inputProfileChunk, BufferString& tempStr) {

	String description;

	inputProfileChunk.getAttribute(L"gameName", description);

	if (description.isEmpty()) {

		assrt(false);
		return;
	}

	String profileName;

	inputProfileChunk.getAttribute(L"name", profileName);

	String selectName;

	inputProfileChunk.getAttribute(L"selectName", selectName);


	typedef WETL::CountedRefArray<Input::Controller*, true, unsigned int> Controllers;
	Controllers controllers;

	typedef WETL::CountedPtrArray<Controllers*, true, unsigned int> ControllerArrays;
	ControllerArrays controllerArrays;

	typedef WETL::CountedArray<bool, false, unsigned int> MarkFlags;
	MarkFlags markFlags;

	unsigned int requiredControllerCount = 0;

	{
		SoftRef<DataChunk> controllerChunk = inputProfileChunk.getChild(L"controller");
		SoftRef<Input::Controller> controller;

		while (controllerChunk.isValid()) {

			bool doMark = true;
			controllerChunk->scanAttribute(tempStr, L"mark", doMark);

			markFlags.addOne(doMark);

			++requiredControllerCount;
			SoftPtr<Controllers> controllers;
			WE_MMemNew(controllers.ptrRef(), Controllers());
			controllerArrays.addOne(controllers);

			String controllerType;
			controllerChunk->getAttribute(L"type", controllerType);

			for (Input::InputManager::Index i = 0; i < manager.getControllerCount(); ++i) {

				controller = manager.getController(i);
				
				if (!controller->isVisible() 
					&& (controllerType.isEmpty() || controllerType.equalsIgnoreCase(controller->getType()))
					&& (!doMark || !controller->isMarked())) {

					SoftRef<DataChunk> filterChunk = controllerChunk->getFirstChild();

					while (controller.isValid() && filterChunk.isValid()) {

						if (filterChunk->equals(L"require")) {

							if (filterChunk->getAttribute(L"name", tempStr)) {

								String deviceName(controller->getName());

								if (deviceName.findNextIgnoreCase(tempStr, 0) < 0) {

									controller.destroy();
								}
							}
						}

						toNextSibling(filterChunk);
					}

					if (!controller.isValid()) {

						controller = manager.getController(i);
						SoftPtr<DeviceInfo> deviceInfo = controller->getDeviceInfo();

						if (deviceInfo.isValid() 
								&& deviceInfo->profile.isValid() 
								&& deviceInfo->profile->equalsIgnoreCase(profileName)) {
							
						} else {

							controller.destroy();
						}
					}

					if (controller.isValid()) {

						controller->AddRef();
						controllers->addOne(controller);
					}
				}
			}
		
			controllerChunk = controllerChunk->getSibling(L"controller");
		}
	}

	if (requiredControllerCount > 1) {

		//implements all combinations of controllers later

		assrt(false);
		return;
	}

	if (controllerArrays.count > 0) {

		Controllers& controllerCombinations = dref(controllerArrays.el[0]);

		for (Controllers::Index i = 0; i < controllerCombinations.count; ++i) {

			Controllers combination;

			//simplistic fill combination (only 1 controller)
			{
				SoftPtr<Input::Controller> controller = controllerCombinations[i];
				controller->AddRef();
				combination.addOne(controller);
			}

			
			SoftRef<Controller_Virtual> controller;
		
			WE_MMemNew(controller.ptrRef(), Controller_Virtual(description.c_tstr(), selectName.c_tstr(), combination[0]->getDeadZone()));

			SoftRef<DataChunk> mapChunk = inputProfileChunk.getFirstChild();
			bool success = true;
			
			while (mapChunk.isValid()) {

				bool checkMarked = true;
				mapChunk->scanAttribute(tempStr, L"checkMarked", checkMarked);

				SoftPtr<Input::Controller> sourceController = combination[0];

				if (mapChunk->getAttribute(L"controller", tempStr)) {

					if (tempStr.equals(L"@self")) {

						sourceController = controller;
						checkMarked = false;

					} else {

						Controllers::Index i = 0;

						if (tempStr.fromString(L"@%u", &i) && i > combination.count) {
							
							sourceController = combination[i];

						} else {

							assrt(false);
							success = false;
							break;
						}
					}
				}

				bool mark = checkMarked;
				mapChunk->scanAttribute(tempStr, L"mark", mark);

				float filterMin = -FLT_MAX;
				float filterMax = FLT_MAX;

				if (mapChunk->equals(L"mapAny")) {

					bool negate = false;
					bool allowDummy = false;

					mapChunk->scanAttribute(tempStr, L"filterMin", L"%f", &filterMin);
					mapChunk->scanAttribute(tempStr, L"filterMax", L"%f", &filterMax);
					mapChunk->scanAttribute(tempStr, L"negate", negate);
					mapChunk->scanAttribute(tempStr, L"allowDummy", allowDummy);

					SourceHolder fromSource;

					if (!getSource(sourceController, NULL, mapChunk, allowDummy, filterMin, filterMax, checkMarked, mark ? controller : NULL, fromSource)) {

						success = false;
						break;
					}

					if (mapChunk->getAttribute(L"to", tempStr)) {

						if (negate) {

							if (!Effect_Negate::create(fromSource, tempStr.c_tstr(), fromSource)) {

								success = false;
								break;
							}
						}

						if (!controller->addSource(fromSource, tempStr.c_tstr())) {

							success = false;
							break;
						}

					} else {

						success = false;
						break;
					}

				} else if (mapChunk->equals(L"map")) {

					bool negate = false;
					bool allowDummy = false;

					mapChunk->scanAttribute(tempStr, L"negate", negate);
					mapChunk->scanAttribute(tempStr, L"allowDummy", allowDummy);


					SourceHolder fromSource;

					if (mapChunk->getAttribute(L"from", tempStr)) {

						if (!getSource(sourceController, &tempStr, mapChunk, allowDummy, filterMin, filterMax, checkMarked, mark ? controller : NULL, fromSource)) {

							success = false;
							break;
						}
					}

					if (mapChunk->getAttribute(L"to", tempStr)) {

						if (negate) {

							if (!Effect_Negate::create(fromSource, tempStr.c_tstr(), fromSource)) {

								success = false;
								break;
							}
						}

						controller->addSource(fromSource, tempStr.c_tstr());

					} else {

						success = false;
						break;
					}

				} else if (mapChunk->equals(L"mapAnd")) {

					bool negate = false;
					mapChunk->scanAttribute(tempStr, L"negate", negate);

					SourceHolder fromSource1;

					if (mapChunk->getAttribute(L"from1", tempStr)) {

						if (!getSource(sourceController, &tempStr, mapChunk, false, filterMin, filterMax, checkMarked, mark ? controller : NULL, fromSource1)) {

							success = false;
							break;
						}
					}

					SourceHolder fromSource2;

					if (mapChunk->getAttribute(L"from2", tempStr)) {

						if (!getSource(sourceController, &tempStr, mapChunk, false, filterMin, filterMax, checkMarked, mark ? controller : NULL, fromSource2)) {

							success = false;
							break;
						}
					}

					if (!fromSource1.isValid() || !fromSource2.isValid()) {

						success = false;
						break;
					}

					if (mapChunk->getAttribute(L"to", tempStr)) {

						SourceHolder andSource;

						if (!Effect_And::create(manager, fromSource1, fromSource2, tempStr.c_tstr(), andSource)) {

							success = false;
							break;
						}

						if (negate) {

							if (!Effect_Negate::create(andSource, tempStr.c_tstr(), andSource)) {

								success = false;
								break;
							}
						}

						if (!controller->addSource(andSource, tempStr.c_tstr())) {

							success = false;
							break;
						}

					} else {

						success = false;
						break;
					}

				} else if (mapChunk->equals(L"mapToAxis2D")) {

					bool negate = false;
					mapChunk->scanAttribute(tempStr, L"negate", negate);

					SourceHolder fromSource1;

					if (mapChunk->getAttribute(L"fromNeg", tempStr)) {

						if (!getSource(sourceController, &tempStr, mapChunk, false, filterMin, filterMax, checkMarked, mark ? controller : NULL, fromSource1)) {

							success = false;
							break;
						}
					}

					SourceHolder fromSource2;

					if (mapChunk->getAttribute(L"fromPos", tempStr)) {

						if (!getSource(sourceController, &tempStr, mapChunk, false, filterMin, filterMax, checkMarked, mark ? controller : NULL, fromSource2)) {

							success = false;
							break;
						}
					}

					if (!fromSource1.isValid() || !fromSource2.isValid()) {

						success = false;
						break;
					}

					if (mapChunk->getAttribute(L"to", tempStr)) {

						SourceHolder andSource;

						if (!Effect_2SourcesToAxis::create(manager, fromSource1, fromSource2, tempStr.c_tstr(), andSource)) {

							success = false;
							break;
						}

						if (negate) {

							if (!Effect_Negate::create(andSource, tempStr.c_tstr(), andSource)) {

								success = false;
								break;
							}
						}

						if (!controller->addSource(andSource, tempStr.c_tstr())) {

							success = false;
							break;
						}

					} else {

						success = false;
						break;
					}
				}
				
				toNextSibling(mapChunk);
			}

			if (success) {

				for (MarkFlags::Index i = 0; i < markFlags.count; ++i) {

					if (markFlags[i]) {

						combination[i]->mark(controller);
					}
				}

				manager.addController(controller);

			} else {

				controller.destroy();
			}
		}
	}
}

void Loader::loadControllerProfiles(InputManager& manager, const TCHAR* inputProfileFolderPath, DataSourcePool& dataSourcePool) {

	if (inputProfileFolderPath) {

		typedef WETL::CountedRefArray<DataChunk*, true, unsigned int> InputProfileChunks;

		InputProfileChunks inputProfileChunks;

		String dirPath(inputProfileFolderPath);
		Win::FileIterator iter;
		BufferString fileName;
		bool isFolder;
		String ext(L".xml");

		BufferString tempStr;

		if (Win::startFileIterate(dirPath.c_tstr(), iter)) {

			do {

				iter.getCurrFileName(fileName, isFolder);

				if (!isFolder && fileName.length() >= 4) {

					if (fileName.findNext(ext, fileName.length() - 4) > 0) {

						fileName.insert(dirPath);

						SoftRef<DataChunk> inputProfileFileChunk = dataSourcePool.getChunk(fileName.c_tstr(), true, NULL, true);

						if (inputProfileFileChunk.isValid()) {

							SoftRef<DataChunk> inputProfileChunk = inputProfileFileChunk->getChild(L"profile");

							while (inputProfileChunk.isValid()) {

								int priority = 0;
								bool wasInserted = false;

								inputProfileChunk->scanAttribute(tempStr, L"priority", L"%d", &priority);

								for (InputProfileChunks::Index i = 0; i < inputProfileChunks.count; ++i) {

									SoftPtr<DataChunk> compProfile = inputProfileChunks[i];

									int compPriority = 0;
									compProfile->scanAttribute(tempStr, L"priority", L"%d", &compPriority);

									if (priority >= compPriority) {
										
										inputProfileChunk->AddRef();
										inputProfileChunks.addOneAt(i, inputProfileChunk);
										wasInserted = true;
										break;
									}
								}

								if (!wasInserted) {

									inputProfileChunk->AddRef();
									inputProfileChunks.addOne(inputProfileChunk);
								}

								inputProfileChunk = inputProfileChunk->getSibling(L"profile");
							}
						}
					}
				}

			} while(Win::nextFileIterate(iter));

			for (Input::InputManager::Index i = 0; i < manager.getControllerCount(); ++i) {

				SoftRef<Input::Controller> controller = manager.getController(i);

				if (controller->isDevice()) {

					controller->setVisible(false);
				}
			}

			for (InputProfileChunks::Index i = 0; i < inputProfileChunks.count; ++i) {

				processInputProfile(manager, dref(inputProfileChunks[i]), tempStr);
			}
		}
	}
}

void Loader::loadDeviceInfos(Input::InputManager& manager, DataChunk& deviceInfosChunk, BufferString& tempStr) {

	SoftRef<DataChunk> deviceInfoChunk = deviceInfosChunk.getChild(L"deviceInfo");

	while (deviceInfoChunk.isValid()) {

		SoftPtr<DeviceInfo> deviceInfo;

		WE_MMemNew(deviceInfo.ptrRef(), DeviceInfo());

		if (!loadDeviceInfo(deviceInfo, deviceInfoChunk, tempStr)) {

			HardPtr<DeviceInfo> deviceInfoDestroy = deviceInfo;

			deviceInfo.destroy();
		}

		if (deviceInfo.isValid()) {

			manager.addDeviceInfo(deviceInfo);
		}

		deviceInfoChunk = deviceInfoChunk->getSibling(L"deviceInfo");
	}
}

bool Loader::loadDeviceInfo(Input::DeviceInfo& deviceInfo, DataChunk& deviceInfoChunk, BufferString& tempStr) {

	deviceInfo.reset();

	if (deviceInfoChunk.getAttribute(L"name", tempStr)) {

		WE_MMemNew(deviceInfo.name.ptrRef(), String(tempStr.c_tstr()));
	}

	if (deviceInfoChunk.getAttribute(L"profile", tempStr)) {

		WE_MMemNew(deviceInfo.profile.ptrRef(), String(tempStr.c_tstr()));
	}

	if (!deviceInfoChunk.scanAttribute(tempStr, L"index", L"%u", &deviceInfo.index)) {

		if (!deviceInfo.name.isValid() || deviceInfo.name->length() == 0)
			return false;
	}

	deviceInfoChunk.scanAttribute(tempStr, L"instanceIndex", L"%u", &deviceInfo.cloneIndex);
	deviceInfoChunk.scanAttribute(tempStr, L"deadZone", L"%f", &deviceInfo.deadZone);
	deviceInfoChunk.scanAttribute(tempStr, L"enable", deviceInfo.enable);

	return true;
}

} }