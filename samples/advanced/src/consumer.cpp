/*!
 * \file     
 * \brief    An advanced yat example
 * \author   N. Leclercq - Synchrotron SOLEIL
 */

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <iostream>
#include "context.h"
#include "consumer.h"

// ============================================================================
// Consumer::Consumer
// ============================================================================
Consumer::Consumer ()
{
	YAT_TRACE("Consumer::Consumer");
}

// ======================================================================
// Consumer::~Consumer
// ======================================================================
Consumer::~Consumer (void)
{
	YAT_TRACE("Consumer::~Consumer");
}

// ============================================================================
// Consumer::handle_message
// ============================================================================
void Consumer::handle_message (yat::Message& _msg)
	throw (yat::Exception)
{
	//- YAT_TRACE("Consumer::handle_message");

	//- handle msg
  switch (_msg.type())
	{
	  //- TASK_INIT ----------------------
	  case yat::TASK_INIT:
	    {
  	    //- "initialization" code goes here
  	    this->index = 0;
      } 
		  break;
		//- TASK_EXIT ----------------------
		case yat::TASK_EXIT:
		  {
  			//- "release" code goes here
      }
			break;
    //- NEW_DATA_AVAILABLE_MSG
    case NEW_DATA_AVAILABLE_MSG:
      {
        //- get data associated with the message 
        size_t no = _msg.get_data<size_t>();
        std::cout << "Consumer::"
                  << this
                  << "::handle_message" 
                  << "::receiving <new objects> notification - " 
                  << no
                  << " new objects in repository" 
                  << std::endl;
        //- parse new objects
        const size_t max = this->index + no;
        std::cout << "Consumer::"
                  << this
                  << "::handle_message" 
                  << "::parsing objs in {" 
                  << Context::instance().data[this->index]->inst_id
                  << ","
                  << Context::instance().data[max - 1]->inst_id
                  << "}" 
                  << std::endl;
        for (; this->index < max;)
          Context::instance().data[this->index++];
      }
      break;
  	default:
  		break;
	}
}

