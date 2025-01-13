#pragma once

#include <iostream>
#include <ostream>
#include <memory>
#include <fstream>
#include <vector>
#include <array>
#include <random>
#include <algorithm>
#include <thread>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <queue>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/System/Vector2.hpp>
#include "imgui.h"
#include "imgui-SFML.h"

struct Complex
{
	double real;
	double imag;

	Complex(double _real = 0, double _imag = 0) :
		real(_real),
		imag(_imag)
	{

	}

	Complex(const Complex& other):
		real(other.real),
		imag(other.imag)
	{

	}

	Complex& operator=(const Complex& other)
	{
		if (this != &other)
		{
			real = other.real;
			imag = other.imag;
		}

		return *this;
	}

	Complex operator+(const Complex& other)
	{
		return Complex(real + other.real, imag + other.imag);
	}

	Complex operator+(const double& realNum) {
		return Complex(real + realNum, imag);
	}
	
	Complex operator-(const Complex& other)
	{
		return Complex(real - other.real, imag - other.imag);
	}

	Complex operator*(const Complex& other)
	{
		return Complex(real * other.real - imag * other.imag, real * other.imag + other.real * imag);
	}

	Complex operator/(const Complex& other)
	{
		double denom = other.real * other.real + other.imag * other.imag;
		if (denom == 0)
		{
			std::cerr << "Error: Division by zero!" << std::endl;
			exit(EXIT_FAILURE);
		}
		double returnreal = (real * other.real + imag * other.imag) /denom;
		double returnimag = (imag * other.real - real * other.imag) /denom;

		return Complex(returnreal, returnimag);
	}

	bool operator==(const Complex& other)
	{
		return (real == other.real) && (imag == other.imag);
	}

	double magnitude()
	{
		return std::sqrt(real * real + imag * imag);
	}

	double sqr_magnitude()
	{
		return (real * real + imag * imag);
	}

	friend std::ostream& operator<<(std::ostream& os, const Complex& c);
};

Complex square(const Complex& c)
{
	return Complex(c.real * c.real - c.imag * c.imag, 2 * c.real * c.imag);
}

std::ostream& operator<<(std::ostream& os, const Complex& c)
{
	os << c.real << "+" << c.imag << "i";
	return os;
}