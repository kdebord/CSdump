#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>



int gcd_euclids(int x, int y, int count);
float average_vector(std::vector<float> set);
float average_mods(std::vector<int> mods);

int main(int argc, char const *argv[])
{
	std::ofstream outputFile;
	outputFile.open("data.R");
	outputFile << "test_num  num_mods average_runtime\n";
	for(int test_num = 1; test_num < 2000; test_num++)
	{	
		std::vector<float> single_test;
		std::vector<int> num_mods;
		for( int n = 1; n < test_num; n++)
		{
			std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
			num_mods.push_back(gcd_euclids(test_num,n,0));
			std::chrono::high_resolution_clock::time_point end_time = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>( end_time - start_time).count();
			std::cout << " Runtime: " << duration << "  nanoseconds\n";
			single_test.push_back(duration);
		}
		std::cout << "\n\n";

		float average = average_vector(single_test);
		float num_mods_avg = average_mods(num_mods);
		outputFile << test_num << " " << num_mods_avg << " " << average << "\n\n";
	}

	outputFile.close();
	return 0;
}

int gcd_euclids(int x, int y, int count)
{
	//returns the total number of modules for (m,n)
	if( y == 0)
		return count;
	return gcd_euclids(y, x % y, count+1);
}

float average_vector(std::vector<float> set)
{
	int num_entries = set.size();
	float sum = 0;
	while(!set.empty())
	{
		float set_num = set.back();
		set.pop_back();
		sum += set_num;
	}
	return (sum/num_entries);
}

float average_mods(std::vector<int> mods)
{
	float num_entries = mods.size();
	float sum = 0;
	while(!mods.empty())
	{
		int set_mod = mods.back();
		mods.pop_back();
		sum += set_mod;
	}
	return (sum/num_entries);
}
