#include <wireless-phy.h>

#define CS_MAX_CODE          999

class LeachWirelessPhy : public WirelessPhy
{
	private:
		double pktEnergy(double pt, double pxcvr, int nbytes);

	protected:
		double *cs_end_;                     // time when carrier sence will end per code

	        double Efriss_amp_;                  // Xmit amp energy (J/bit/m^2)
	        double Etwo_ray_amp_;                // Xmit amp energy (J/bit/m^4)
	        double EXcvr_;                       // Xcvr energy (J/bit)
	        double Pfriss_amp_;                  // Friss base transmission power (W/m^2)
	        double Ptwo_ray_amp_;                // Two-ray base transmission power (W/m^4)
	        double PXcvr_;                       // Xcvr Power (W)
		double dist_;                        // approx. distance to transmitter
		int ss_;                             // amount of spreading


	public:
		LeachWirelessPhy();
		~LeachWirelessPhy();

		inline double csEnd(int code)
		{
			if (code > CS_MAX_CODE || code < 0)
			{
				fprintf(stderr, "csEnd(code): code > CS_MAX_CODE?; %d %d\n", code, CS_MAX_CODE);
				exit(-1);
			}

			return cs_end_[code];
		}

		int sendUp(Packet * p);
		void sendDown(Packet * p);
};
