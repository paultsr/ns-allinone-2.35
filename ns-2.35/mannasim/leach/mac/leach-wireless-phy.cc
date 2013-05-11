#include "leach-wireless-phy.h"

static class LeachWirelessPhyClass : public TclClass
{
	public:
		LeachWirelessPhyClass() : TclClass("Phy/WirelessPhy/LeachWirelessPhy") {}

		TclObject* create(int, const char * const *)
		{
			return new LeachWirelessPhy();
		}
} class_LeachWirelessPhy;

/*****************************************************************************/

LeachWirelessPhy::LeachWirelessPhy()
{
	cs_end_ = new double[CS_MAX_CODE + 1];

	for (int i = 0; i < CS_MAX_CODE + 1; i++)
		cs_end_[i] = 0;

        bandwidth_ = 1000000;                // 100 Mbps
        Efriss_amp_ = 100 * 1e-12;           // Friss amp energy (J/bit/m^2)
        Etwo_ray_amp_ = 0.013 * 1e-12;       // Two-ray amp energy (J/bit/m^4)
        EXcvr_ = 50 * 1e-9;                  // Xcvr energy (J/bit)	
        // Use this base threshold to get a "hearing radius" of ~ 1 m
	//         //   Pfriss_amp_ = Efriss_amp_ * bandwidth_;      // Friss power (W/m^2)
        Ptwo_ray_amp_ = Etwo_ray_amp_ * bandwidth_;  // Two-ray power (W/m^4)
        PXcvr_ = EXcvr_ * bandwidth_;        // Xcvr power (W)
        ss_ = 1;                             // amount of spreading
//        time_finish_rcv_ = 0;
        dist_ = 0;                           // approx. distance to transmitter
}

LeachWirelessPhy::~LeachWirelessPhy()
{
	delete [] cs_end_;
}

int LeachWirelessPhy::sendUp(Packet *p)
{
	double Pr;
	int pk_recvd = 0;
	PacketStamp s;

	if(propagation_)
	{
                s.stamp((MobileNode*)node(), ant_, 0, lambda_);
                Pr = propagation_->Pr(&p->txinfo_, &s, this);
	}

	hdr_cmn *ch = HDR_CMN(p);
	hdr_rca *rca_hdr = HDR_RCA(p);

	// Record when this packet ends and its code.
	int code = rca_hdr->get_code();

//	printf("Tempo de recebimento para pacote: %lf\n", txtime(p));
	cs_end_[code] = Scheduler::instance().clock() + txtime(p);

	pk_recvd = WirelessPhy::sendUp(p);

	if (pk_recvd)
	{	
		/*
		** Determine approximate distance of node transmitting node
		** from received power.
		**/
		double hr, ht;        // height of recv and xmit antennas
		double rX, rY, rZ;    // receiver location
		double d1, d2;
		double crossover_dist, Pt, M;

		((MobileNode *) node_)->getLoc(&rX, &rY, &rZ);

		hr = rZ + ant_->getZ();
		ht = hr;              // assume transmitting node antenna at same height

		crossover_dist = sqrt((16 * PI * PI * L_ * ht * ht * hr * hr)
				/ (lambda_ * lambda_));

		Pt = p->txinfo_.getTxPr();
		M = lambda_ / (4 * PI);
		d1 = sqrt( (Pt * M * M) / (L_ * Pr) );
		d2 = sqrt(sqrt( (Pt * hr * hr * ht * ht) / Pr) );

		if (d1 < crossover_dist)
			dist_ = d1;
		else
			dist_ = d2;

		rca_hdr->dist_est() = (int) ceil(dist_);
	}

	return pk_recvd;
}

void LeachWirelessPhy::sendDown(Packet * p)
{
	if ((node()->energy_model()->node_on() != true) || (node()->energy_model()->sleep()))
	{
		printf("Node is not on or it is sleeping!\n");
		return;
	}

	/*
	** The power for transmission depends on the distance between
	** the transmitter and the receiver.  If this distance is
	** less than the crossover distance:
	**       (c_d)^2 =  16 * PI^2 * L * hr^2 * ht^2
	**               ---------------------------------
	**                           lambda^2
	** the power falls off using the Friss equation.  Otherwise, the
	** power falls off using the two-ray ground reflection model.
	** Therefore, the power for transmission of a bit is:
	**      Pt = Pfriss_amp_*d^2 if d < c_d
	**      Pt = Ptwo_ray_amp_*d^4 if d >= c_d.
	** The total power dissipated per bit is PXcvr_ + Pt.
	**/
	hdr_cmn *ch = HDR_CMN(p);
	hdr_rca *rca_hdr = HDR_RCA(p);

	double d = rca_hdr->get_dist();
        double hr, ht;        // height of recv and xmit antennas
	double tX, tY, tZ;    // transmitter location

	((MobileNode *) node_)->getLoc(&tX, &tY, &tZ);

	ht = tZ + ant_->getZ();
	hr = ht;              // assume receiving node and antenna at same height

	double crossover_dist = sqrt((16 * PI * PI * L_ * ht * ht * hr * hr)
	                                           / (lambda_ * lambda_));

	if (d < crossover_dist)
		if (d > 1)
			Pt_ = Efriss_amp_ * bandwidth_ * d * d;
          	else
			// Pfriss_amp_ is the minimum transmit amplifier power.
	                Pt_ = Efriss_amp_ * bandwidth_;
        else
		Pt_ = Etwo_ray_amp_ * bandwidth_ * d * d * d * d;

	PXcvr_ = EXcvr_ * bandwidth_;

	// if (energy_->remove(pktEnergy(Pt_, PXcvr_, ch->size())) != 0)
	//	alive_ = 0;

	Pt_consume_ = pktEnergy(Pt_, PXcvr_, ch->size());

	WirelessPhy::sendDown(p);
}

double LeachWirelessPhy::pktEnergy(double pt, double pxcvr, int nbytes)
{
	/*
	** Energy (in Joules) is power (in Watts=Joules/sec) divided by
	** bandwidth (in bits/sec) multiplied by the number of bytes, times 8 bits.
	**/
	// If data has been spread, power per DATA bit should be the same
	// as if there was no spreading ==> divide transmit power
	// by spreading factor.

	double bits = (double) nbytes * 8;

	pt /= ss_;

	double j = bits * (pt + pxcvr) / bandwidth_;

	return j;
}

