#include <ns-process.h>

class LeachAdvCHData : public AppData
{
	private:
		int nodeid_;

	public:
		LeachAdvCHData(int nodeid) : AppData(LEACH_ADV_CH)
		{
			this->nodeid_ = nodeid;
		}

		inline int & nodeid() { return nodeid_; }
};
