#include "heartDefines.h"
#include <QObject>

//heartDefines::heartDefines()
//{
//	saNodeSize_1d = 0;
//	atriumSize_1d = 0;
//	avNodeSize_1d = 0;
//	totalSize_1d = 0;
//}
//void heartDefines::setSaSize(int size)
//{
//	saNodeSize_1d = size;
//	totalSize_1d = saNodeSize_1d + avNodeSize_1d + atriumSize_1d;
//}
//void heartDefines::setAvSize(int size)
//{
//	avNodeSize_1d = size;
//	totalSize_1d = saNodeSize_1d + avNodeSize_1d + atriumSize_1d;
//}
//void heartDefines::setAtriumSize(int size)
//{
//	atriumSize_1d = size;
//	totalSize_1d = saNodeSize_1d + avNodeSize_1d + atriumSize_1d;
//}
//void heartDefines::setBranchAvSize(int size)
//{
//	branchAvSize = size;
//	branchLength = branchAvSize + branchAtriumSize;
//}
//void heartDefines::setBranchAtriumSize(int size)
//{
//	branchAtriumSize = size;
//	branchLength = branchAvSize + branchAtriumSize;
//}
//void heartDefines::setBranchStart(int pos)
//{
//	branchStart = pos;
//
//}
//void heartDefines::setBranchEnd(int pos)
//{
//	branchEnd = pos;
//
//}
//void heartDefines::validateBranchPosition()
//{
//	if( branchStart <= saNodeSize_1d || branchStart >= ( saNodeSize_1d += atriumSize_1d ) )
//	{
//		branchStart = saNodeSize_1d + 1;
//	}
//
//	if( branchStart >= totalSize_1d || branchStart <= ( saNodeSize_1d += atriumSize_1d ) )
//	{
//		branchEnd = totalSize_1d - 1;
//	}
//}
//void heartDefines::toggleBranching(bool tog)
//{
//	branchingToggle = tog;
//}
