#include <stdlib.h>

struct costMatrixRow {
  int x;
  int y;
};
void foo(int channelNets) {
  _Array_ptr<struct costMatrixRow> costMatrix = malloc<struct costMatrixRow>(
      (channelNets + 1) * sizeof(struct costMatrixRow));
  int i = 0;
  for (; i < channelNets; i++) {
    costMatrix[i].x = 0;
    costMatrix[i].y = 0;
  }
}
