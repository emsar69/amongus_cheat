#pragma once
#include <fstream>

namespace logger {
	inline std::ofstream logfile;

	inline void Init() {
		logfile = std::ofstream("Logs.txt");
	}

	inline void Write(const char* text) {
		logfile << text << std::endl;
	}

	inline void Write(float text) {
		logfile << text << std::endl;
	}

	inline void Write(DWORD text){
		logfile << text << std::endl;
	}

	inline void Write(int text) {
		logfile << text << std::endl;
	}

	inline void Write(uintptr_t* text) {
		logfile << text << std::endl;
	}

	inline void Write(uintptr_t text) {
		logfile << std::hex << text << std::endl;
	}

	inline void Write(std::string text) {
		logfile << text << std::endl;
	}

	inline void Destroy() {
		logfile.close();
	}
}