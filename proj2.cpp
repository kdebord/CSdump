#include <cmath>
#include <iostream>
#include <string>

int makeEqualLength(std::string &num1, std::string &num2);
int singleDigitMultiplication(const std::string& number1, const std::string& number2);
std::string addStrings(std::string& string1, std::string& string2);
long int karatsuba(std::string num1, std::string num2);
long int exponentiation(int num1, int num2);


int main()
{
    int num1, num2;
    std::cout << "Enter 1st Number: ";
    std::cin >> num1;
    std::cout << "Enter 2nd Number: ";
    std::cin >> num2;
    long long result = karatsuba(std::to_string(num1), std::to_string(num2));
    std::cout << num1 << " * " << num2 << " = " << result;
    if(result == num1 * num2)
        std::cout << "\nCorrect!";
    else
        std::cout << "\nFalse!";

    return 0;
}

// Method to make strings of equal length
int makeEqualLength(std::string &num1, std::string &num2)
{
    int length1 = num1.length();
    int length2 = num2.length();

    //determine which string is shorter and fill with leading 0's to match
    if(length1 < length2)
        return (num1.insert(0, length2 - length1, '0'), length2);
    else
        return (num2.insert(0, length1 - length2, '0'), length1);

}

int singleDigitMultiplication(const std::string& number1, const std::string& number2)
{
    //return int result of single digit mult of two strings.
    //handles if numbers have extra leading zeros
    return ((number1[0] - '0') * (number2[0] - '0'));
}

std::string addStrings(std::string& string1, std::string& string2)
{
    //basic addition set up using strings as input
    int length = makeEqualLength(string1, string2);
    int carry = 0;
    std::string sum;
    for (int i = length - 1; i >= 0; i--)
    {
        int first = string1[i] - '0';
        int second = string2[i] - '0';
        int temp = (first + second + carry) % 10 + '0';
        carry = (first + second + carry) / 10;
        sum.insert(0, 1, temp);
    }
    if (carry != 0)
        sum.insert(0, 1, (carry + '0'));
    return sum;
}

long int karatsuba(std::string num1, std::string num2)
{
    //Karatsuba based off psuedocode implementation from wikipedia page, altered to apply to strings
    int length = makeEqualLength(num1, num2);
    if (length == 0) return 0;
    if (length == 1) return singleDigitMultiplication(num1, num2);

    int first_num = length / 2;
    int second_num = length - first_num;

    std::string A_left_half = num1.substr(0, first_num);
    std::string A_right_half = num1.substr(first_num, second_num);
    std::string B_left_half = num2.substr(0, first_num);
    std::string B_right_half = num2.substr(first_num, second_num);

    long int Z1 = karatsuba(A_left_half, B_left_half);
    long int Z2 = karatsuba(A_right_half, B_right_half);
    long int Z3 = karatsuba(addStrings(A_left_half, A_right_half), addStrings(B_left_half, B_right_half)) - Z1 - Z2;
    long int result = Z1 * pow(10, 2 * second_num) + Z3 * pow(10, second_num) + Z2;

    return result;
}

long int exponentiation(int num1, int num2)
{

}
