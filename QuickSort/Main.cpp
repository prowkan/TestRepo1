#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <fstream>

#define GENERATE_AND_SAVE_ARRAYS 1
#define LOAD_ARRAYS_FROM_FILES 2

#define MODE LOAD_ARRAYS_FROM_FILES

void quickSort(std::vector<int>& array)
{
	struct StackElement
	{
		int left, right;
	};

	std::vector<StackElement> stack;
	stack.reserve(array.size());
	stack.push_back(StackElement{ 0, int(array.size() - 1) });

	while (stack.size() > 0)
	{
		StackElement stackElement = stack[stack.size() - 1];
		stack.pop_back();

		int pivotElement = array[(stackElement.left + stackElement.right) / 2];

		int i = stackElement.left;
		int j = stackElement.right;

		while (i <= j)
		{
			while (i <= j && array[i] < pivotElement) i++;
			while (i <= j && array[j] > pivotElement) j--;

			if (i <= j)
			{
				int temp = array[i];
				array[i] = array[j];
				array[j] = temp;

				i++;
				j--;
			}
		}

		if (stackElement.left < j) stack.push_back(StackElement{ stackElement.left, j });
		if (i < stackElement.right) stack.push_back(StackElement{ i, stackElement.right });
	}
}

bool checkIsArraySorted(const std::vector<int>& array)
{
	size_t arraySize = array.size();

	for (int i = 0; i < arraySize - 1; i++)
	{
		if (array[i] > array[i + 1]) return false;
	}

	return true;
}

void generateRandomArray(std::vector<int>& array, const int numElements, const int minElement, const int maxElement)
{
	array.reserve(numElements);

	std::random_device randomDevice;
	std::mt19937_64 generator(randomDevice());
	std::uniform_int_distribution<int> distribution(minElement, maxElement);

	for (int i = 0; i < numElements; i++)
	{
		array.push_back(distribution(generator));
	}
}

void writeArrayToFile(const std::string& fileName, const std::vector<int>& array)
{
	std::fstream outputFile(fileName, std::ios::out);

	for (const int arrayElement : array)
	{
		outputFile << arrayElement << " ";
	}
}

void readArrayFromFile(const std::string& fileName, std::vector<int>& array, int assumedArraySize = 0)
{
	if (assumedArraySize) array.reserve(assumedArraySize);

	std::fstream inputFile(fileName, std::ios::in);

	while (true)
	{
		int arrayElement = 0;

		inputFile >> arrayElement;

		if (inputFile.eof()) break;

		array.push_back(arrayElement);
	}
}

int main()
{
	int arraySizes[] = { 10'000, 100'000, 1'000'000 };

	struct Range
	{
		int minValue, maxValue;
	};

	Range arrayRanges[] = { { -10, 10 }, { -1'000, 1'000 }, { -100'000, 100'000 } };

	const int numRuns = 5;

	const int arraySizesCount = _countof(arraySizes);
	const int arrayRangesCount = _countof(arrayRanges);

	std::vector<int> testArrays[arraySizesCount][arrayRangesCount];

	const char* fileNames[arraySizesCount][arrayRangesCount] =
	{
		{ "Array_10000_10.txt", "Array_10000_1000.txt", "Array_10000_100000.txt" },
		{ "Array_100000_10.txt", "Array_100000_1000.txt", "Array_100000_100000.txt" },
		{ "Array_1000000_10.txt", "Array_1000000_1000.txt", "Array_1000000_100000.txt" },
	};

#if MODE == GENERATE_AND_SAVE_ARRAYS
	for (int arraySizeIndex = 0; arraySizeIndex < arraySizesCount; arraySizeIndex++)
	{
		for (int arrayRangeIndex = 0; arrayRangeIndex < arrayRangesCount; arrayRangeIndex++)
		{
			std::vector<int>& array = testArrays[arraySizeIndex][arrayRangeIndex];

			generateRandomArray(array, arraySizes[arraySizeIndex], arrayRanges[arrayRangeIndex].minValue, arrayRanges[arrayRangeIndex].maxValue);

			writeArrayToFile(fileNames[arraySizeIndex][arrayRangeIndex], array);
		}
	}
#elif MODE == LOAD_ARRAYS_FROM_FILES
	for (int arraySizeIndex = 0; arraySizeIndex < arraySizesCount; arraySizeIndex++)
	{
		for (int arrayRangeIndex = 0; arrayRangeIndex < arrayRangesCount; arrayRangeIndex++)
		{
			std::vector<int>& array = testArrays[arraySizeIndex][arrayRangeIndex];

			readArrayFromFile(fileNames[arraySizeIndex][arrayRangeIndex], array, arraySizes[arraySizeIndex]);
		}
	}
#endif

	for (int arraySizeIndex = 0; arraySizeIndex < arraySizesCount; arraySizeIndex++)
	{
		for (int arrayRangeIndex = 0; arrayRangeIndex < arrayRangesCount; arrayRangeIndex++)
		{
			std::vector<int>& array = testArrays[arraySizeIndex][arrayRangeIndex];

			std::cout << "Testing array of " << arraySizes[arraySizeIndex] << " elements (range: [" << arrayRanges[arrayRangeIndex].minValue << ", " << arrayRanges[arrayRangeIndex].maxValue << "]): ";

			unsigned long long totalDuration = 0;

			for (int runIndex = 0; runIndex < numRuns; runIndex++)
			{
				const std::chrono::time_point timeBegin = std::chrono::high_resolution_clock::now();

				quickSort(array);

				const std::chrono::time_point timeEnd = std::chrono::high_resolution_clock::now();

				totalDuration += std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeBegin).count();
			}

			std::cout << (totalDuration / 1000.0f) / float(numRuns) << " ms. ";

			std::cout << "Checking array for being sorted: ";

			if (checkIsArraySorted(array)) std::cout << "Successful." << std::endl;
			else std::cout << "FAILED." << std::endl;
		}
	}

	return 0;
}