#pragma once

#include <iostream>

namespace libtrainsim {

    /**
     * @brief an abstraction on whatever backend is used for handling frames.
     * 
     */
    class Frame{
        private:
            

        public:

            /**
             * @brief Construct an empty frame.
             * 
             */
            Frame();
            
            /**
             * free all of the data in this Frame object.
             */
            virtual void clear();
            
            /**
             * clear the data of the frame when it is destroyed
             */
            virtual ~Frame();

            /**
             * @brief Checks if the frame data is empty
             * 
             * @return true the frame data is empty
             * @return false the frame data has contents
             */
            virtual bool isEmpty() const;
    };

}
