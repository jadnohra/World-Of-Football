#include "PhysGraph.h"

namespace WOF { namespace match {

float Graph::getValue(float x, LineGraphPoint* pGraph, unsigned int graphSize) {

	if (graphSize == 0) {
		return 0.0f;
	}
	

	unsigned int i;
	for (i = 0; i < graphSize; i++) {
		if (x <= pGraph[i].data[0]) {
			break;
		}
	}

	if (i == 0) {
		return pGraph[i].data[1];
	} 

	if (i >= graphSize) {
		i = graphSize - 1;
	}
	

	if (pGraph[i].nextLineVertical) {
		return pGraph[i + 1].data[1];
	}
	return pGraph[i].nextLineA * x + pGraph[i].nextLineB;
}

void Graph::prepare(LineGraphPoint* pGraph, unsigned int graphSize) {
	
	if (graphSize == 0) {
		return;
	}
	
	unsigned int i;
	for (i = 0; i < graphSize - 1; i++) {
		if (pGraph[i].data[0] == pGraph[i + 1].data[0]) {
			pGraph[i].nextLineVertical = true;
		} else {
			pGraph[i].nextLineVertical = false;
			pGraph[i].nextLineA = (pGraph[i].data[1] - pGraph[i + 1].data[1]) / (pGraph[i].data[0] - pGraph[i + 1].data[0]);
			pGraph[i].nextLineB = pGraph[i].data[1] - pGraph[i].nextLineA * pGraph[i].data[0];
		}
	}

	i =  graphSize - 1;
	pGraph[i].nextLineVertical = false;
	pGraph[i].nextLineA = 0.0f;
	pGraph[i].nextLineB = pGraph[i].data[1];
}

} }