#ifndef h_WOF_match_PhysGraph
#define h_WOF_match_PhysGraph

#include "WE3/WEMem.h"

namespace WOF { namespace match {

	struct LineGraphPoint {

		float data[2];

		bool nextLineVertical;
		float nextLineA;
		float nextLineB;
	};

	class Graph {
	public:

		static void prepare(LineGraphPoint* pGraph, unsigned int graphSize);
		static float getValue(float x, LineGraphPoint* pGraph, unsigned int graphSize);
	};


	#define PhysMaxGraphSize 6

	struct PhysGraph {

		unsigned int graphSize;
		LineGraphPoint graph[PhysMaxGraphSize];

		float effectCoeff;

		PhysGraph() : effectCoeff(1.0f) {

			graphSize = 0;
			memset(graph, 0, sizeof(graph));
		}

		void prepare(unsigned int size) {

			graphSize = size;
			Graph::prepare(graph, graphSize);
		}
	};

} }

#endif