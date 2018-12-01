#include "SpatialMatchState.h"

#include "../entity/footballer/Footballer.h"
#include "../entity/ball/Ball.h"

#include "../Match.h"
#include "../FootballerIteratorImpl.h"

namespace WOF { namespace match {

SpatialMatchState::BallDist::BallDist() {
}

void SpatialMatchState::BallDist::init(Match& match) {
}

void SpatialMatchState::BallDist::update(Match& match, const Time& time) {

	bool needsFullUpdate = mFootballerIteratorIsDirty;

	if (mFootballerIteratorIsDirty) {

		FootballerIndex footballerCount = mFootballerIterator->getFootballerCount();
		mFootballerIteratorIsDirty = false;

		mFootballers.reserve(footballerCount);
	}

	if (!mFootballerIterator.isValid()) {

		return;
	}

	bool hasUpdates = false;

	if (needsFullUpdate) {

		hasUpdates = true;
		mFootballers.reset();

		FootballerIterator& iter = mFootballerIterator.dref();
		SoftPtr<Footballer> footballer;
		FootballerIndex index;
			
		while (iter.iterateFootballers(footballer.ptrRef(), index)) {

			bool wasInserted = false;

			for (FootballerIndex i = 0; i < mFootballers.count; ++i) {

				SoftPtr<Footballer> compFootballer = mFootballers[i];

				float distDiff = footballer->getBallInteraction().relativeDist.getFootballerDist() - compFootballer->getBallInteraction().relativeDist.getFootballerDist(); 
				bool isBetter = distDiff <= 0.0f;

				if (isBetter) {
				
					mFootballers.addOneAtReserved(i, footballer);
					footballer->getSpatialState().indexBallDist = i;
					wasInserted = true;

					++i;
					for (; i < mFootballers.count; ++i) {

						mFootballers[i]->getSpatialState().indexBallDist = i;
					}

					break;
				}
			}

			if (!wasInserted) {

				footballer->getSpatialState().indexBallDist = mFootballers.count;
				mFootballers.addOneReserved(footballer);
			}
		}

	} else {

		if (mFootballers.count) {

			for (FootballerIndex i = 0; i < mFootballers.count; ++i) {

				SoftPtr<Footballer> footballer = mFootballers[i];
				float footballerDist = footballer->getBallInteraction().relativeDist.getFootballerDist();
				FootballerState& state = footballer->getSpatialState();
				FootballerIndex footballerIndex = i;

				bool moved = false;

				for (FootballerIndex j = i + 1; j < mFootballers.count; ++j) {

					SoftPtr<Footballer> compFootballer = mFootballers[j];
					FootballerState& compState = compFootballer->getSpatialState();

					float distDiff = footballerDist - compFootballer->getBallInteraction().relativeDist.getFootballerDist(); 

					if (distDiff <= 0.0f) {

						break;

					} else {

						hasUpdates = true;
						mFootballers[j] = footballer;
						state.indexBallDist = j;
						mFootballers[footballerIndex] = compFootballer;
						compState.indexBallDist = footballerIndex;
						footballerIndex = j;
					}
				}

				if (!moved && i > 0) {

					for (FootballerIndex j = i - 1; j != (FootballerIndex) -1 ; --j) {

						SoftPtr<Footballer> compFootballer = mFootballers[j];
						FootballerState& compState = compFootballer->getSpatialState();

						float distDiff = footballerDist - compFootballer->getBallInteraction().relativeDist.getFootballerDist(); 

						if (distDiff >= 0.0f) {

							break;

						} else {

							hasUpdates = true;
							mFootballers[j] = footballer;
							state.indexBallDist = j;
							mFootballers[footballerIndex] = compFootballer;
							compState.indexBallDist = footballerIndex;
							footballerIndex = j;
						}
					}
				}
			}
		}
	}

	if (hasUpdates) {

		mTeamFootballers[Team_A].reset();
		mTeamFootballers[Team_B].reset();

		mTeamFootballers[Team_A].reserve(match.getTeam(Team_A).getFootballerCount());
		mTeamFootballers[Team_B].reserve(match.getTeam(Team_B).getFootballerCount());

		for (FootballerIndex i = 0; i < mFootballers.count; ++i) {

			SoftPtr<Footballer> footballer = mFootballers[i];

			mTeamFootballers[footballer->mTeam].addOne(footballer);
		}
	}

}

} }