/* Implemented by Julio Cesar e Melo <cmelo_bhz@hotmail.com> */

#ifndef _COMMONNODEDIFFAPP_H_
#define _COMMONNODEDIFFAPP_H_

#include <list>
#include <stdlib.h>

#include "nrAttributes.h"
#include "attrsExporter.h"
#include "../commonNodeApp.h"

using namespace std;

/* CommonNodeDiffApp
 * -- J�lio C�sar e Melo
 *
 * This is a class for a common sensor node.
 * It inherits from CommonNodeApp (from MannaSIM)
 * and DiffApp. As both are applications and inherit
 * from TclObject, there are some ambiguous virtual
 * functions. To solve this problem, it was implemented
 * an adaptor for calling the specific functions of
 * each ancestor (see CommonNodeApp::TclObjectAdaptor).
 */
class CommonNodeDiffApp : public DiffApp, CommonNodeApp
{
	private:
		class SensedDataInterestReceiver;

	public:
		/* Type definitions */
		struct PublishedData
		{
			handle             hnd;
			AppDataType        appDataType;
			int                subscriptions;
			NR::Callback *     receiver;
		};
	
		typedef list<PublishedData *> PublishedDataList;
		typedef list<NRAttrVec *>       NRAVList;

		// Subscribe for receiving interest message
		void subscribePublishedData(PublishedData *);

	protected:
		// List of published datas
		PublishedDataList publishedDataList_;

		// List of attribute list
		NRAVList subscriptions_;

		// Publish an application data type
		PublishedData * publishData(AppDataType appDataType, NR::Callback * receiver = NULL);
		PublishedData * publishData(AppDataType appDataType, AttrsExporter * attrsExporter);
	
		// Find a published data
		PublishedData * findPublishedData(AppDataType appDataType);

		// Occurs when received a subscription
		virtual void onSubscription(PublishedData *, NRAttrVec *);

		// Occurs when received a unsubscription
		virtual void onUnsubscription(PublishedData *, NRAttrVec *);

		// Check if there is someone interested
		bool interested(NRAttrVec *);

	public:
		// Public constructors and destructors
		CommonNodeDiffApp();
		~CommonNodeDiffApp();
	
		virtual void start();
		virtual void run();
	
		// Disseminate processed data from sensor node
		virtual void disseminateData(SensedData * data_);
	
		// Attach a new data generator
		virtual void insertNewGenerator(DataGenerator * gen);

		/*****************************/
		/***** Ambiguous methods *****/
		/*****************************/

		virtual int command(int argc, const char * const * argv)
		{
			int result;

			result = CommonNodeApp::command(argc, argv);

			if (result == TCL_OK)
				return result;

			result = DiffApp::command(argc, argv);

			return result;
		}

		virtual void trace(TracedVar *tv)
		{
			CommonNodeApp::trace(tv);
		}

		virtual int init(int argc, const char * const * argv)
		{
			CommonNodeApp::init(argc, argv);
		}

		virtual int delay_bind_dispatch(const char * varName, const char * localName, TclObject * tracer)
		{
			CommonNodeApp::delay_bind_dispatch(varName, localName, tracer);
		}

		virtual void delay_bind_init_all()
		{
			CommonNodeApp::delay_bind_init_all();
		}

                /* Adaptor - Necessary to remove ambiguous from base
                 * TclObject class
                 */
		class TclObjectAdaptor : public TclObject
		{
		private:
			CommonNodeDiffApp * obj_;

		public:
			TclObjectAdaptor()
			{
				obj_ = new CommonNodeDiffApp();
			}

			~TclObjectAdaptor()
			{
				delete obj_;
			}

			virtual int command(int argc, const char * const * argv)
			{
				obj_->command(argc, argv);
			}

			virtual void trace(TracedVar *tv)
			{
				obj_->trace(tv);
			}

			virtual int init(int argc, const char * const * argv)
			{
				obj_->init(argc, argv);
			}

			virtual int delay_bind_dispatch(const char * varName, const char * localName, TclObject * tracer)
			{
				obj_->delay_bind_dispatch(varName, localName, tracer);
			}

			virtual void delay_bind_init_all()
			{
				obj_->delay_bind_init_all();
			}
		};

	private:
		/* Interest receiver */
		class InterestReceiver : public NR::Callback
		{
			private:
				PublishedData     * pubData_;
				CommonNodeDiffApp * node_;

			public:
				InterestReceiver(
					CommonNodeDiffApp *, PublishedData *);

				void recv(NRAttrVec *, NR::handle);
		};
};
	
#endif
