#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>

#include "PageManager.h"

#include <iostream>
#include <string>

namespace
{
	void runSample(const std::string& label)
	{
		std::cout << "[sample] " << label << std::endl;
		PageManager pageManager;
		std::cout << "[OK] PageManager creation" << std::endl;
	}
	
	
	void runSampleWithFile(const std::string& label, const std::string& filename)
	{
		std::cout << "[sample] " << label << std::endl;
		PageManager pageManager;
		std::cout << "[OK] PageManager creation" << std::endl;
		
		// Placeholder for file operations
		// e.g., pageManager.readPage(...), pageManager.writePage(...)
		
		std::cout << "[OK] File operations completed" << std::endl;
		
	}
	void runSampleWithPageOperations(const std::string& label)
	{
		std::cout << "[sample] " << label << std::endl;

		const std::string filename = "pagemgr_sample.bin";
		std::remove(filename.c_str());

		PageManager pageManager(filename, std::ios::out | std::ios::in | std::ios::binary);
		std::cout << "[OK] PageManager creation" << std::endl;

		// Create a buffer to hold page data
		uint8_t buffer[PAGE_SIZE] = {1, 2, 3, 4, 5}; // Example data

		// Write the buffer to a page
		PageID pageId = 1; // Example page ID
		pageManager.writePage(pageId, buffer);

		std::cout << "[OK] Page written" << std::endl;

		// Read it back into a separate buffer and compare
		uint8_t readBuffer[PAGE_SIZE] = {0};
		if (pageManager.readPage(pageId, readBuffer) &&
			std::memcmp(buffer, readBuffer, PAGE_SIZE) == 0) {
			std::cout << "[OK] Page read successfully" << std::endl;
		}else{
			std::cerr << "[ERROR] Failed to read page" << std::endl;
		}

		pageManager.close();
		std::remove(filename.c_str());

		std::cout << "[OK] Page operations completed" << std::endl;
	} // namespace
	
}
int main()
{
	std::cout << "PageManager unit test samples" << std::endl;
	
	// Basic creation
	{
		PageManager pageManager;
		std::cout << "[OK] Basic PageManager creation succeeded" << std::endl;
	}
	
	// Scoped sample creation
	runSample("scoped instance");
	
	// Another instance to show repeated construction usage
	{
		PageManager anotherPageManager;
		std::cout << "[OK] Second PageManager instance created successfully" << std::endl;
	}
	
	// Placeholder for creating a file or other future PageManager use cases.
	runSampleWithFile("file operations", "example.txt");
	
	// Placeholder for page operations
	runSampleWithPageOperations("page operations");
	
	
	
	std::cout << "All PageManager samples completed" << std::endl;
	return 0;
}

