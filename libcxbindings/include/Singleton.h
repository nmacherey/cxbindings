/**
 * @file Singleton.h
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
#ifndef SINGLETON_H
#define SINGLETON_H

#include <cassert>

/* @class Singleton
 *
 * @brief Singleton is the class used for simulating static manager.
 *
 * To use this class simply inherit your object from Singleton<YourModule> and instanciate
 * instance and isShutdown static variables for the Singleton<YourModule> then you will
 * have access to your object global instance.
 * YourModule::Get().
 *
 * @note this class has been taken from the Code::Blocks IDE
 *
 * @author Nicolas Macherey ( nicolas.macherey@gmail.com )
 * @date 02-Dec-2008
 * @version 0.0.1
 */
template <class MgrT> class Singleton
{
    static MgrT *instance;										/*!< global instance for the module */
    static bool isShutdown;										/*!< is the module shutting down */

    explicit Singleton(const Singleton<MgrT>&){};							/*!< copy constructor */
    Singleton<MgrT>& operator=(Singleton<MgrT> const&){return *this;};		/*!< operator = overload */

protected:

    Singleton(){assert(Singleton<MgrT>::instance == 0);}					/*!< Default constructor */
    virtual ~Singleton(){Singleton<MgrT>::instance = 0;}					/*!< Default destructor */

public:

	/*! Specifies is the manager is valid or not */
	static inline bool Valid(){return (instance!=0) ? true : false;}

	/*! Use this function to have access to the managers global instance */
    static inline MgrT* Get()
    {
        if(instance == 0 && isShutdown == false)
            instance = new MgrT();

        return instance;
    }

	/*! NEVER call this function, it called by the application when closing */
    static void Free()
    {
        isShutdown = true;
        delete instance;
        instance = 0;
    }

    static inline bool IsDown() {return isShutdown;};
};

#endif
