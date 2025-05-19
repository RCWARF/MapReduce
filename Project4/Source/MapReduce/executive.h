#include <string>
#include "workflow.h"
#pragma once

class Executive
{
	public:
		Executive(std::string target_dir, 
			std::string intermediate_dir,
			std::string out_dir,
			int num_mappers,
			int num_reducers,
			int controller_port,
			std::vector<std::string> stub_endpoints);

		~Executive();
		// Calls workflow to run
		void startWorkflow();

	private:
		Workflow* workflow_;
};
