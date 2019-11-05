#include "Real.h"
#include <sstream>
#include <iomanip>
#include <vector>
#include <algorithm>

#define decimal 50

Real::Real ()
{
	sign = 'x';
	whole = nullptr;
	fraction = nullptr;
}

Real::~Real ()
{
	while(!number.empty())
	{
		number.pop_back();
	}
}

Real::Real (const Real & R)
{
	this->sign = R.sign;
	this->number = R.number;
}

Real::Real (const string & S)
{
	unsigned long long len = S.length();
	char i;
	if(S[0] == '-')
	{
		sign = '-';
		i = 1;
	}
	else
	{
		sign = '+';
		i = 0;
	}

	bool has_decimal = false;
	for (i; i < len; i++) {
		switch(S[i])
		{
			case '0':
				number.push_back(0);
				break;
			case '1':
				number.push_back(1);
				break;
			case '2':
				number.push_back(2);
				break;
			case '3':
				number.push_back(3);
				break;
			case '4':
				number.push_back(4);
				break;
			case '5':
				number.push_back(5);
				break;
			case '6':
				number.push_back(6);
				break;
			case '7':
				number.push_back(7);
				break;
			case '8':
				number.push_back(8);
				break;
			case '9':
				number.push_back(9);
				break;
			case '.':
				number.push_back(decimal);
				has_decimal = true;
				break;
		    default:
		        i = len - 1;
		        if(number.size() == 0)
                {
		            number.push_back(0);
                }
		        break;

		}
	}

    if(has_decimal == true)
    {
        while(number.back() == 0)
        {
            number.pop_back();
        }
        if(number.back() == decimal)
            number.pop_back();
    }

	while(*number.begin() == 0 && number.size() > 1)
    {
	    number.erase(number.begin());
    }


}

Real & Real::operator = (const Real & R)
{

	this->number = R.number;
	this->sign = R.sign;
	return *this;

}

Real Real::operator + (const Real & R) const
{
	Real temp1(*this);
	Real temp2(R);


	if((temp1.sign == '+' && temp2.sign == '+') || (temp1.sign == '-' && temp2.sign == '-'))
	{
		pad_numbers(temp1, temp2);
		return add_nums(temp1, temp2);
	}

	if(temp1.number.size() < temp2.number.size())
	{
		Real r = temp1;
		temp1 = temp2;
		temp2 = r;
	}
	if(temp1.number.size() == temp2.number.size())
	{
		auto iter1 = temp1.number.begin();
		auto iter2 = temp2.number.begin();
		for( ; iter1 != temp1.number.end(); iter1++, iter2++)
		{
			if (*iter1 < *iter2 == true)
			{
				Real r = temp1;
				temp1 = temp2;
				temp2 = r;
				break;
			}
		}

	}

	//we know now its one number thats negative and one thats positive
	//larger number is set to the "top" position

	pad_numbers(temp1, temp2);
	string sum = "";
	auto iter2 = temp2.number.end() - 1;
	for(auto iter1 = temp1.number.end() - 1; iter1 != temp1.number.begin() - 1; iter2--, iter1--)
	{
		if(*iter1 == decimal)
		{
			iter1--;
			iter2--;
			sum += '.';
		}
		if(*iter1 < *iter2)
		{
			auto tempiter = iter1 - 1;
			while(*tempiter == 0)
				tempiter--;
			while(tempiter!= iter1)
			{
				auto dis = distance(temp1.number.begin(), tempiter);
				temp1.number[dis] = temp1.number[dis] - 1;
				temp1.number[dis + 1] = temp1.number[dis + 1] + 10;
				tempiter++;
			}
		}
		sum += to_string(*iter1 - *iter2);
	}
	reverse(sum.begin(), sum.end());
	Real tempr(sum);
	if(temp1.sign == '-')
		tempr.sign = '-';
	return tempr;

//	string comp = "";
//	int temp = 0;
//	bool first_nonzero = false;
//
//	for(auto iter = temp2.number.end() - 1; iter != temp2.number.begin() - 1; iter--)
//	{
//		if(*iter == 0 && first_nonzero == false)
//			comp = '0' + comp;
//		else if(first_nonzero == false)
//		{
//			temp = 10 - *iter;
//			first_nonzero = true;
//			comp = to_string(temp) + comp;
//		}
//		else if(first_nonzero == true)
//		{
//			temp = 9 - *iter;
//			comp = to_string(temp) + comp;
//		}
//	}
//	while(comp.size() < temp1.number.size())
//	{
//		comp = '9' + comp;
//	}
//
//	Real tempr(comp);
//	char temp_sign = temp1.sign;
//	temp1.sign = '+';
//	pad_numbers(temp1, temp2);
//	Real sum(add_nums(temp1, tempr));
//	sum.number.erase(sum.number.begin());
//	if(temp_sign == '-' && sum.number[sum.number.back()] != 0)
//		sum.sign = '-';
//	return sum;

	//if temp1 was negative the result is negative



}

ostream & operator << (ostream & outs, const Real & R)
{
	if(R.sign == '-')
		outs << '-';
	unsigned long long len = R.number.size();
	for (auto i = 0; i < len; i++) {
		if(R.number[i] == decimal)
			outs << '.';
		else
			outs << to_string(R.number[i]);
	}
	return outs;
}

void Real::pad_numbers(Real &R1, Real &R2) const
{
	auto pos1 = find(R1.number.begin(), R1.number.end(), decimal);
	auto pos2 = find(R2.number.begin(), R2.number.end(), decimal);

	if(*pos1 == decimal && *pos2 != decimal)
		R2.number.push_back(decimal);

	if(*pos1 != decimal && *pos2 == decimal)
		R1.number.push_back(decimal);

	//both contain a decimal, needs padding
	pos1 = find(R1.number.begin(), R1.number.end(), decimal);
	pos2 = find(R2.number.begin(), R2.number.end(), decimal);
	auto dis1 = distance(R1.number.begin(), pos1);
	auto dis2 = distance(R2.number.begin(), pos2);
	dis1 = R1.number.size() - dis1 - 1;
	dis2 = R2.number.size() - dis2 - 1;
	if(dis1 < dis2)
	{
		for(auto i = dis1; i != dis2; i++)
		{
			R1.number.push_back(0);
		}
	}
	else
	{

		for(auto i = dis2; i != dis1; i++)
		{
			R2.number.push_back(0);
		}
	}
	if(R1.number.size() < R2.number.size())
	{
		auto diff = R2.number.size() - R1.number.size();
		while (diff > 0) {
			R1.number.insert(R1.number.begin(), 0);
			diff--;
		}
	}
	else
	{
		auto diff = R1.number.size() - R2.number.size();
		while (diff > 0) {
			R2.number.insert(R2.number.begin(), 0);
			diff--;
		}
	}

	return;
}

Real Real::add_nums(Real &temp1, Real &temp2) const
{
	string sum = "";
	unsigned long long len;
	int carry = 0;

	int temp;
	auto iter1 = temp1.number.end() - 1;
	auto iter2 = temp2.number.end() - 1;
	for(iter1; iter1 != temp1.number.begin() - 1; iter1--, iter2--)
	{
		if(*iter1 == decimal)
		{
			sum += '.';
			iter1--;
			iter2--;
		}
		if(*iter1 + *iter2 + carry > 9)
		{
			temp = *iter1 + *iter2 + carry;
			temp = temp % 10;
			carry = 1;
			sum += to_string(temp);
		}
		else
		{
			temp = *iter1 + *iter2 + carry;
			carry = 0;
			sum += to_string(temp);
		}
	}
	if(carry == 1)
		sum += '1';

	if(temp1.sign == '-' && temp2.sign == '-')
		sum += '-';

	reverse(sum.begin(), sum.end());
	while(*sum.end() == '0')
		sum = sum.substr(0, sum.size() - 1);
//	while(*(sum.begin()) == '0')
//		sum = sum.substr(1, sum.size());

	Real r(sum);
	return r;

}
