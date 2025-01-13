#include "headers/header.hpp"

class mandelbrot
{
private:
	int numX;
	int numY;
	int num;

	int topbound = 2;
	int startbound = -2;
	double bound = 4;
	int maxiter = 2048;

	std::vector<Complex> set;
	std::vector<int> inset;

	std::mutex mtx;

	bool usegoodcolor;
	bool save;

public:

	std::atomic<int> currentidx = 0;

	std::vector<uint8_t> bitmap;
	
	mandelbrot(int X, int Y, bool _usegoodcolor, bool _save):
		numX(X),
		numY(Y),
		usegoodcolor(_usegoodcolor),
		save(_save)
	{
		num = numX * numY;
		set.resize(num);
		inset.resize(num, maxiter);
	}

	void threaded_emplacement()
	{
		while (true)
		{
			int i = currentidx.fetch_add(1);
			if (i >= set.size())
			{
				break;
			}

			Complex c = set[i];
			Complex z = Complex(0, 0);
			for (int iter = 1; iter < maxiter; iter++)
			{
				if (z.sqr_magnitude() >= bound)
				{
					inset[i] = iter;
					break;
				}
				z = z * z + c;
			}
		}
		std::cout << "Thread done.\n";
	}

	void calculate(std::vector<std::thread>& threads)
	{
		for (int i = 0; i < numX; i++)
		{
			for (int j = 0; j < numY; j++)
			{
				int index = i * numY + j;
				double imag = startbound + (j * ((topbound - startbound) / static_cast<double>(numY)));
				double real = (i * ((topbound - startbound) / static_cast<double>(numX))) - topbound;
				set[index] = Complex(real, imag);
			}
		}

		std::cout << "done complex mapping \n";

		for (int i = 0; i < threads.size(); i++)
		{
			threads[i] = std::thread(&mandelbrot::threaded_emplacement, this);
		}

		for (auto& t : threads)
		{
			if (t.joinable())
			{
				t.join();
			}
		}

		std::cout << "done mandelbrot calculating \n";

	}

	void draw(sf::Image& image, std::string& filename)
	{
		if (save)
		{
			uint16_t  type = 0x4D42;
			uint16_t  reserved1 = 0;
			uint16_t  reserved2 = 0;
			uint32_t  offset = 54;
			uint32_t  dib_header_size = 40;
			int32_t   width_px = numX;
			int32_t   height_px = numY;
			uint16_t  num_planes = 1;
			uint16_t  bits_per_pixel = 24;
			uint32_t  compression = 0;
			uint32_t  image_size_bytes = numX * numY * 3;
			uint32_t  size = image_size_bytes + 54;

			float image_aspect_ratio = static_cast<float>(numX) /
				static_cast<float>(numY);

			float print_width_meters = 0.2f;

			float print_height_meters = print_width_meters / image_aspect_ratio;

			int32_t x_resolution_ppm = static_cast<int32_t>(numX / print_width_meters);
			int32_t y_resolution_ppm = static_cast<int32_t>(numX / print_height_meters);

			uint32_t  num_colors = 0;
			uint32_t  important_colors = 0;

			bitmap.push_back((uint8_t)(type & 0xFF));
			bitmap.push_back((uint8_t)((type >> 8) & 0xFF));

			bitmap.push_back((uint8_t)(size & 0xFF));
			bitmap.push_back((uint8_t)((size >> 8) & 0xFF));
			bitmap.push_back((uint8_t)((size >> 16) & 0xFF));
			bitmap.push_back((uint8_t)((size >> 24) & 0xFF));

			bitmap.push_back((uint8_t)(reserved1 & 0xFF));
			bitmap.push_back((uint8_t)((reserved1 >> 8) & 0xFF));

			bitmap.push_back((uint8_t)(reserved2 & 0xFF));
			bitmap.push_back((uint8_t)((reserved2 >> 8) & 0xFF));

			bitmap.push_back((uint8_t)(offset & 0xFF));
			bitmap.push_back((uint8_t)((offset >> 8) & 0xFF));
			bitmap.push_back((uint8_t)((offset >> 16) & 0xFF));
			bitmap.push_back((uint8_t)((offset >> 24) & 0xFF));

			bitmap.push_back((uint8_t)(dib_header_size & 0xFF));
			bitmap.push_back((uint8_t)((dib_header_size >> 8) & 0xFF));
			bitmap.push_back((uint8_t)((dib_header_size >> 16) & 0xFF));
			bitmap.push_back((uint8_t)((dib_header_size >> 24) & 0xFF));

			bitmap.push_back((uint8_t)((width_px) & 0xFF));
			bitmap.push_back((uint8_t)(((width_px) >> 8) & 0xFF));
			bitmap.push_back((uint8_t)(((width_px) >> 16) & 0xFF));
			bitmap.push_back((uint8_t)(((width_px) >> 24) & 0xFF));

			bitmap.push_back((uint8_t)((height_px) & 0xFF));
			bitmap.push_back((uint8_t)(((height_px) >> 8) & 0xFF));
			bitmap.push_back((uint8_t)(((height_px) >> 16) & 0xFF));
			bitmap.push_back((uint8_t)(((height_px) >> 24) & 0xFF));

			bitmap.push_back((uint8_t)(num_planes & 0xFF));
			bitmap.push_back((uint8_t)((num_planes >> 8) & 0xFF));

			bitmap.push_back((uint8_t)(bits_per_pixel & 0xFF));
			bitmap.push_back((uint8_t)((bits_per_pixel >> 8) & 0xFF));

			bitmap.push_back((uint8_t)(compression & 0xFF));
			bitmap.push_back((uint8_t)((compression >> 8) & 0xFF));
			bitmap.push_back((uint8_t)((compression >> 16) & 0xFF));
			bitmap.push_back((uint8_t)((compression >> 24) & 0xFF));

			bitmap.push_back((uint8_t)(image_size_bytes & 0xFF));
			bitmap.push_back((uint8_t)((image_size_bytes >> 8) & 0xFF));
			bitmap.push_back((uint8_t)((image_size_bytes >> 16) & 0xFF));
			bitmap.push_back((uint8_t)((image_size_bytes >> 24) & 0xFF));

			bitmap.push_back((uint8_t)((x_resolution_ppm) & 0xFF));
			bitmap.push_back((uint8_t)(((x_resolution_ppm) >> 8) & 0xFF));
			bitmap.push_back((uint8_t)(((x_resolution_ppm) >> 16) & 0xFF));
			bitmap.push_back((uint8_t)(((x_resolution_ppm) >> 24) & 0xFF));

			bitmap.push_back((uint8_t)((y_resolution_ppm) & 0xFF));
			bitmap.push_back((uint8_t)(((y_resolution_ppm) >> 8) & 0xFF));
			bitmap.push_back((uint8_t)(((y_resolution_ppm) >> 16) & 0xFF));
			bitmap.push_back((uint8_t)(((y_resolution_ppm) >> 24) & 0xFF));

			bitmap.push_back((uint8_t)(num_colors & 0xFF));
			bitmap.push_back((uint8_t)((num_colors >> 8) & 0xFF));
			bitmap.push_back((uint8_t)((num_colors >> 16) & 0xFF));
			bitmap.push_back((uint8_t)((num_colors >> 24) & 0xFF));

			bitmap.push_back((uint8_t)(important_colors & 0xFF));
			bitmap.push_back((uint8_t)((important_colors >> 8) & 0xFF));
			bitmap.push_back((uint8_t)((important_colors >> 16) & 0xFF));
			bitmap.push_back((uint8_t)((important_colors >> 24) & 0xFF));
		}

		sf::Color mapping[16];
		mapping[0] = sf::Color(70, 30, 10, 255);
		mapping[1] = sf::Color(25, 5, 25, 255);
		mapping[2] = sf::Color(10, 0, 50, 255);
		mapping[3] = sf::Color(5, 5, 75, 255);
		mapping[4] = sf::Color(0, 5, 100, 255);
		mapping[5] = sf::Color(15, 45, 140, 255);
		mapping[6] = sf::Color(25, 85, 180, 255);
		mapping[7] = sf::Color(60, 125, 210, 255);
		mapping[8] = sf::Color(135, 185, 230, 255);
		mapping[9] = sf::Color(215, 240, 250, 255);
		mapping[10] = sf::Color(245, 235, 195, 255);
		mapping[11] = sf::Color(250, 200, 95, 255);
		mapping[12] = sf::Color(255, 170, 0, 255);
		mapping[13] = sf::Color(205, 130, 0, 255);
		mapping[14] = sf::Color(155, 90, 0, 255);
		mapping[15] = sf::Color(105, 5, 5, 255);

		for (int j = numY - 1; j >= 0; j--)
		{
			for (int i = 0; i < numX; i++)
			{

				int idx = i * numY + j;
				
				if (usegoodcolor)
				{
					sf::Color color;

					int n = inset[idx];
					if (n < maxiter && n > 0) {
						int i = n % 16;
						color = mapping[i];
					}
					else
					{
						color = sf::Color::Black;
					}
					uint8_t r = color.r;
					uint8_t g = color.g;
					uint8_t b = color.b;


					image.setPixel(i, j, color);

					if (save)
					{
						bitmap.push_back(b);
						bitmap.push_back(g);
						bitmap.push_back(r);
					}
				}
				else
				{
					double map360 = (inset[idx] / static_cast<double>(maxiter)) * 360.0;

					int h = static_cast<int>(std::pow(map360, 1.5)) % 360;
					int s = 1;
					int v = (inset[idx] / maxiter);

					double c = v * s;
					int x = c * (1 - std::abs(((h / 60) % 2) - 1));
					double m = v - c;

					double rt;
					double gt;
					double bt;

					if (h >= 300)
					{
						rt = c;
						bt = 0;
						gt = x;
					}
					else if (h >= 240)
					{
						rt = 0;
						bt = c;
						gt = x;
					}
					else if (h >= 180)
					{
						rt = 0;
						bt = x;
						gt = c;
					}
					else if (h >= 120)
					{
						rt = 0;
						bt = c;
						gt = x;
					}
					else if (h >= 60)
					{
						rt = x;
						bt = c;
						gt = 0;
					}
					else
					{
						rt = c;
						bt = x;
						gt = 0;
					}

					uint8_t r = (rt + m) * 255;
					uint8_t g = (gt + m) * 255;
					uint8_t b = (bt + m) * 255;

					sf::Color color = sf::Color(r, g, b, 255);
					image.setPixel(i, j, color);

					if (save)
					{
						bitmap.push_back(b);
						bitmap.push_back(g);
						bitmap.push_back(r);
					}
				}

			}
		}

		if (save)
		{
			std::ofstream outputFile(filename, std::ios::binary);

			if (outputFile.is_open()) {
				outputFile.write(reinterpret_cast<const char*>(bitmap.data()), bitmap.size());
				outputFile.close();
			}
			else
			{
				std::cerr << "Error: Could not open output file." << std::endl;
			}
		}

		std::cout << "done drawing \n";

		set.erase(set.begin(), set.end());
		inset.erase(inset.begin(), inset.end());
		bitmap.erase(bitmap.begin(), bitmap.end());
	}
};

void checkinput(sf::RenderWindow& window, float& zoomFactor, int& deltaY, int& deltaX, sf::View& view, int& resX, int& resY)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		zoomFactor += 0.02;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && zoomFactor > 0.01f) {
		zoomFactor -= 0.02;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		deltaY -= 10 * zoomFactor;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		deltaY += 10 * zoomFactor;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		deltaX -= 10 * zoomFactor;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		deltaX += 10 * zoomFactor;
	}
	view.setSize(resX * zoomFactor, resY * zoomFactor);
	view.setCenter(resX / 2 + deltaX, resY / 2 + deltaY);
}

int main()
{
	std::ifstream IfFile;

	bool usegoodcolor;
	bool save;

	std::cout << "Use Custom Colors?\n";
	while (!(std::cin >> usegoodcolor) || (usegoodcolor != 0 && usegoodcolor != 1)) 
	{
		std::cout << "Invalid input. Please enter 1 for custom colors or 0 if not.\n";
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	std::cout << "Save?\n";
	while (!(std::cin >> save) || (save != 0 && save != 1)) 
	{
		std::cout << "Invalid input. Please enter 1 for saving to bmp or 0 if not.\n";
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	std::string filename;

	std::cout << "Input your folder. use double backslashes thanks! Yes you will still have to input this even if you do not choose save! Will work on that.\n";

	std::cin >> filename;

	if (!std::filesystem::exists(filename)) {
		std::filesystem::create_directory(filename);
	}

	filename += "\\output.bmp";


	uint32_t max_threads = std::thread::hardware_concurrency();
	std::vector<std::thread> threads;
	threads.resize(max_threads);
	int resX = 1200;
	int resY = 1200;

	int mandelbrotX = 24000;
	int mandelbrotY = 24000;

	sf::Image image;
	image.create(mandelbrotX, mandelbrotY, sf::Color::Black);
	mandelbrot* mb = new mandelbrot(mandelbrotX, mandelbrotY, usegoodcolor, save);
	mb->calculate(threads);
	mb->draw(image, filename);
	delete mb;

	sf::RenderWindow window = sf::RenderWindow({ static_cast<uint32_t>(resX), static_cast<uint32_t>(resY) }, "Mandlebrot", sf::Style::Titlebar | sf::Style::Close);
	sf::View view(sf::FloatRect(0, 0, resX, resY));
	view.setCenter(resX / 2, resY / 2);
	window.setView(view);

	float zoomFactor = 1;
	int deltaY = 0;
	int deltaX = 0;

	window.setFramerateLimit(170);

	sf::Texture texture;
	texture.loadFromImage(image);

	sf::Sprite sprite(texture);


	while (window.isOpen())
	{
		window.clear(sf::Color::White);

		for (auto event = sf::Event(); window.pollEvent(event);)
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::R))
			{
				view.setCenter(resX / 2, resY / 2);
				zoomFactor = 1;
				deltaY = 0;
				deltaX = 0;
			}
		}

		checkinput(window, zoomFactor, deltaY, deltaX, view, resX, resY);
		window.setView(view);

		window.draw(sprite);
		window.display();

	}
	return 0;
}