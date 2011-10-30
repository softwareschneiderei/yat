/*!
 * \file     
 * \brief    An advanced yat example
 * \author   N. Leclercq - Synchrotron SOLEIL
 */

// ============================================================================
// Dependencies
// ============================================================================
#include <iostream>
#include "context.h"
#include "producer.h"

// ============================================================================
// Producer::Producer
// ============================================================================
Producer::Producer ()
{
	YAT_TRACE("Producer::Producer");
}

// ======================================================================
// Producer::~Producer
// ======================================================================
Producer::~Producer (void)
{
	YAT_TRACE("Producer::~Producer");
}

// ============================================================================
// Producer::handle_message
// ============================================================================
void Producer::handle_message (yat::Message& _msg)
	throw (yat::Exception)
{
	//- YAT_TRACE("Producer::handle_message");

	//- handle msg
  switch (_msg.type())
	{
	  //- TASK_INIT ----------------------
	  case yat::TASK_INIT:
	    {
  	    //- "initialization" code goes here
        this->enable_periodic_msg(true);
        this->set_periodic_msg_period(10);
      } 
		  break;
		//- TASK_EXIT ----------------------
		case yat::TASK_EXIT:
		  {
  			//- "release" code goes here
      }
			break;
		//- TASK_PERIODIC ------------------
		case yat::TASK_PERIODIC:
		  {
  		  //- code relative to the task's periodic job goes here
  		  //- produce some data, push it into the global repository
  		  const size_t no = 1000;
        std::cout << "Producer::handle_message::producing " << no << " objects" << std::endl;
        for (size_t i = 0; i < no; i++)
          Context::instance().data.push_back( Object::instanciate() );
        //- tell the consumers how many new objects are available 
        std::cout << "Producer::handle_message::notifying the consumers" << std::endl;
        const size_t nc = Context::instance().consumers.size();
        yat::Message * m = new yat::Message(NEW_DATA_AVAILABLE_MSG);
        m->attach_data(no);
        //- post the msg and the associated data to each consumer
        for (size_t i = 0; i < nc; i++)
          Context::instance().consumers[i]->post(m->duplicate());
        //- release our msg reference
        m->release();
      }
		  break;
  	default:
  		//- unhandled msg received - just ignore it
  		break;
	}
}

