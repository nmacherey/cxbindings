
/**
 * @file stdSingleton.h
 * @author Nicolas Macherey (nm@graymat.fr)
 * @version 10.4
 */
#ifndef STD_SINGLETON_H
#define STD_SINGLETON_H

#include <cassert>

/* @class stdSingleton
 *
 * @brief stdSingleton is the class used for simulating static manager.
 *
 * To use this class simply inherit your object from stdSingleton<YourModule> and instanciate
 * instance and isShutdown static variables for the stdSingleton<YourModule> then you will
 * have access to your object global instance.
 * YourModule::Get().
 *
 * @note this class has been taken from the Code::Blocks IDE
 *
 * @author Nicolas Macherey ( nm@graymat.fr )
 * @date 02-Dec-2008
 * @version 0.0.1
 */
template <class MgrT> class stdSingleton
{
    static MgrT *instance;										/*!< global instance for the module */
    static bool isShutdown;										/*!< is the module shutting down */

    explicit stdSingleton(const stdSingleton<MgrT>&){};							/*!< copy constructor */
    stdSingleton<MgrT>& operator=(stdSingleton<MgrT> const&){return *this;};		/*!< operator = overload */

protected:

    stdSingleton(){assert(stdSingleton<MgrT>::instance == 0);}					/*!< Default constructor */
    virtual ~stdSingleton(){stdSingleton<MgrT>::instance = 0;}					/*!< Default destructor */

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
		