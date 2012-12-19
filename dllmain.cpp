#include "dllmain.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <memory.h>
#include <time.h>

#include "medriver.h"

#include "src_timing/Struct_Algorithm.h"
#include "TestConsoleGeneric.h"

// a sample exported function



#define MAX_RANGES	 16
struct SAORange
{
    int		m_iUnit;

    double	m_dPhysicalMin;
    double	m_dPhysicalMax;

    int		m_iDigitalMax;
};
struct SAOSubddevice
{
    int	m_iDeviceIndex;
    int m_iSubdeviceIndex;
    int m_iNumberOfChannels;
    int m_iNumberOfRanges;
    SAORange m_arrRanges[MAX_RANGES];
    int m_iCurrentRange;
};
struct SDigitalInSubddevice
{
    int	m_iDeviceIndex;
    int m_iSubdeviceIndex;
    int m_iType;
    int m_iWidth;
};

int utilBuildAOSub(SAOSubddevice *AOSubdevice,
                   int index_device,
                   int index_subdevice
);

int DLL_EXPORT AO(double loop, double sample_rate, char* conf_path)
{
	// Call generic console function in parent directory to
	// open system and list the available devices and subdevices.
    //double  loop = 0;
    //double  sample_rate = 20; //20k
    //char    conf_path[] = "MOTconf.csv";

	int i_number_of_devices = TestConsoleMEIDSProlog();

	if(i_number_of_devices <= 0)
		return(-1);

	// ---------------------------------------------------------------------
	int i_number_ao_subdevices = 16;
	int i_number_di_dio_subdevices = 2;
	int i_me_error;

	// Make a list of the AO sub-devices
	SAOSubddevice* arrSubdevices = new SAOSubddevice[i_number_ao_subdevices];
	// Make a list of the DI sub-devices
	SDigitalInSubddevice* arrSubdevicesDigital = new SDigitalInSubddevice[i_number_di_dio_subdevices];

	for(int index_device = 0; index_device < i_number_of_devices; index_device++)
	{
		int i_vendor_id;
		int i_device_id;
		int i_serial_no;
		int i_bus_type;
		int i_bus_no;
		int i_dev_no;
		int i_func_no;
		int i_plugged;

		i_me_error = meQueryInfoDevice(	index_device,				// Device index
										&i_vendor_id,				// Vendor ID returned here - not required
										&i_device_id,				// Device ID returned here - not required
										&i_serial_no,				// Serial number returned here - not required
										&i_bus_type,				// Bus type returned here - not required
										&i_bus_no,					// Bus number returned here - not required
										&i_dev_no,					// Bus device number returned here - not required
										&i_func_no,					// Bus function number returned here - not required
										&i_plugged				);	// Plugged status returned hee - not required

		if(i_plugged != ME_PLUGGED_IN)
			continue;

		int index_subdevice = 0;
		int i_number_of_subdevices;
		i_number_ao_subdevices = 0;
		i_me_error = meQueryNumberSubdevices(	index_device,				// Device index
												&i_number_of_subdevices);	// Number of subdevices returned here

        // The while-loop building the AO subdevice structure for each ao subdevice
		while(true)
		{
			i_me_error = meQuerySubdeviceByType(index_device,					// Device
												index_subdevice,				// Begin search at this sub-device
												ME_TYPE_AO,						// Type of sub-device to search for
												ME_SUBTYPE_ANY,					// Sub-type of sub-device to search for
												&index_subdevice	);			// sub-device index returned here
			if(i_me_error != ME_ERRNO_SUCCESS)
				break;

            int myError = utilBuildAOSub(&arrSubdevices[i_number_ao_subdevices],
                                         index_device,
                                         index_subdevice);
			if(myError == -1)
                goto error;

            ++i_number_ao_subdevices;
			if(++index_subdevice >= i_number_of_subdevices)
				break;
		}

		// The while-loop building the DI subdevice strcture for each di subdevice
		index_subdevice = 0;
		i_number_di_dio_subdevices = 0;
		while(true)
		{
			i_me_error = meQuerySubdeviceByType(index_device,					// Device
												index_subdevice,				// Begin search at this sub-device
												ME_TYPE_DIO,						// Type of sub-device to search for
												ME_SUBTYPE_ANY,					// Sub-type of sub-device to search for
												&index_subdevice	);			// sub-device index returned here

			if(i_me_error != ME_ERRNO_SUCCESS)
				break;

			arrSubdevicesDigital[i_number_di_dio_subdevices].m_iDeviceIndex = index_device;
			arrSubdevicesDigital[i_number_di_dio_subdevices].m_iSubdeviceIndex = index_subdevice;
			arrSubdevicesDigital[i_number_di_dio_subdevices].m_iType = ME_TYPE_DIO;

			i_me_error = meQueryNumberChannels(	index_device,										// Device
												index_subdevice,									// Sub-device
												&arrSubdevicesDigital[i_number_di_dio_subdevices].m_iWidth	);	// Number of channels returned here

			++i_number_di_dio_subdevices;
			if(++index_subdevice >= i_number_of_subdevices)
				break;
		}
	}

    // print the info of each subdevice
	printf("%d AO sub-devices found: in System\n\n", i_number_ao_subdevices);
	for(int index_subdevice = 0; index_subdevice < i_number_ao_subdevices; index_subdevice++)
	{
		printf("%2d.   Device: %2d  Sub-device: %2d  Channels: %2d  Ranges: %2d\n",
				index_subdevice + 1,
				arrSubdevices[index_subdevice].m_iDeviceIndex,
				arrSubdevices[index_subdevice].m_iSubdeviceIndex,
				arrSubdevices[index_subdevice].m_iNumberOfChannels,
				arrSubdevices[index_subdevice].m_iNumberOfRanges		);
	}
	// print the info of each digital subdevice
	printf("\n%d DI or DIO sub-devices found:\n\n", i_number_di_dio_subdevices);
	for(int index_subdevice = 0; index_subdevice < i_number_di_dio_subdevices; index_subdevice++)
	{
		printf("%2d.   Device: %2d  Sub-device: %2d  Type: %s  Width: %2d Bits\n",
				index_subdevice + 1,
				arrSubdevicesDigital[index_subdevice].m_iDeviceIndex,
				arrSubdevicesDigital[index_subdevice].m_iSubdeviceIndex,
				(arrSubdevicesDigital[index_subdevice].m_iType == ME_TYPE_DIO) ? "DIO" : " DI",
				arrSubdevicesDigital[index_subdevice].m_iWidth										);
	}
//// ******************The End of Acquiring the subdevices information********************* ////
//// ************The following is the configuration and the main output part.************** ////
    while(true)
    {
        // Configure the device
		// ME_TRIG_CHAN_SYNCHRONOUS for simultaneous outputing, ME_TRIG_TYPE_SW for software trigger
        int index_subdevice = 0;   // this should be iterated
        int index_channel = 0;     // there is just one channel in analog output
        int index_range = 0;       // there is only one range
        int i_trigger_channel = ME_TRIG_CHAN_SYNCHRONOUS;  //i_trigger_channel = ME_TRIG_CHAN_DEFAULT,  ME_TRIG_CHAN_SYNCHRONOUS;
        int i_trigger_type = ME_TRIG_TYPE_SW;              //i_trigger_type = ME_TRIG_TYPE_SW,  ME_TRIG_TYPE_EXT_DIGITAL;
        int i_trigger_edge = ME_TRIG_EDGE_RISING;          //i_trigger_edge = ME_VALUE_NOT_USED,  ME_TRIG_EDGE_RISING,  ME_TRIG_EDGE_FALLING,  ME_TRIG_EDGE_ANY
        for(index_subdevice = 0; index_subdevice < i_number_ao_subdevices; index_subdevice++)
        {
            i_me_error = meIOSingleConfig(	arrSubdevices[index_subdevice].m_iDeviceIndex,			// Device index
                                            arrSubdevices[index_subdevice].m_iSubdeviceIndex,		// Subdevice index,
                                            index_channel,											// Channel index
                                            index_range,											// Range index
                                            ME_REF_AO_GROUND,										// Reference
                                            i_trigger_channel,										// Trigger channel - standard
                                            i_trigger_type,											// Trigger type - software
                                            i_trigger_edge,											// Trigger edge - not applicable
                                            ME_IO_SINGLE_CONFIG_NO_FLAGS						);	// Flags
            if(i_me_error != ME_ERRNO_SUCCESS)
            {
                printf("\n****    meIOSingleConfig - Error: %d    ****\n\n", i_me_error);
                goto error;
            }
        }

		// configure digital part
		int index_subdevice_d = 0;
		i_me_error = meIOSingleConfig(	arrSubdevicesDigital[index_subdevice_d].m_iDeviceIndex,		// Device index
										arrSubdevicesDigital[index_subdevice_d].m_iSubdeviceIndex,	// Subdevcie index
										0,													// Channel
										ME_SINGLE_CONFIG_DIO_INPUT,							// Single config
										ME_VALUE_NOT_USED,									// Reference
										ME_TRIG_CHAN_DEFAULT,								// Trigger channel
										ME_TRIG_TYPE_NONE,									// Trigger type
										ME_VALUE_NOT_USED,									// Trigger edge
										ME_IO_SINGLE_CONFIG_DIO_BYTE     ); // Flags
		if(i_me_error != ME_ERRNO_SUCCESS)
		{
			printf("\n****    meIOSingleConfig - Error: %d    ****\n\n", i_me_error);
			goto error;
		}


        // Fetch the range information which is required to convert from
        // physical units to a digital value for the output below
        double d_phys_min;
        double d_phys_max;
        int i_digital_max;
        index_subdevice = 0; // =0 is in fact for all 16 subdevices of ME6100, because they have the same range, or more precisely, the same profile.
        i_me_error = meQueryRangeInfo(	arrSubdevices[index_subdevice].m_iDeviceIndex,			// Device index
                                        arrSubdevices[index_subdevice].m_iSubdeviceIndex,		// Subdevice index,
                                        arrSubdevices[index_subdevice].m_iCurrentRange,			// Range index
                                        NULL,													// Unit returned here - not require
                                        &d_phys_min,											// Physical minimum returned here
                                        &d_phys_max,											// Physical maximum returned here
                                        &i_digital_max										);	// Digital maximum value returned here
        if(i_me_error != ME_ERRNO_SUCCESS)
        {
            printf("****    meQueryRangeInfo - Error: %d    ****\n\n", i_me_error);
            goto error;
        }
        printf("max:%.5fV,  min:%.5fV\n",d_phys_max,d_phys_min);


        // ==== This is the Streamming data ======================================================
        int     t = clock();
        int     myError;
		int     samples_each;
		int     channels_num;
		double  *ret;

        myError = Timing( /* intputs: */ loop,  sample_rate, conf_path,
                          /* outputs: */ &samples_each, &channels_num, &ret );

        int **iDataOfChannel = (int**) malloc(channels_num*sizeof(int*));
        int  *iData          = (int*)  malloc(channels_num*samples_each*sizeof(int));

        for(int i = 0; i < channels_num; ++i)
            iDataOfChannel[i] = &iData[i*samples_each];

        for(int i = 0, j=samples_each*channels_num; i < j; ++i)
        {
            i_me_error = meUtilityPhysicalToDigital(d_phys_min,				// Minimum physiacal value
                                                    d_phys_max,				// Maximum physical value
                                                    i_digital_max,			// Maximum digital value
                                                    ret[i],	                // Physical value to convert
                                                    &iData[i]	);          // Corresponding digital value for meIOSingle returned here
            if(i_me_error != ME_ERRNO_SUCCESS)
            {
                printf("****    meUtilityPhysicalToDigital - Error: %d    ****\n\n", i_me_error);
                free(iDataOfChannel);
                free(iData);
                goto error;
            }
        }
        t = clock()-t;
        printf("Generating timing: %.2fms\n", (double)t/(double)CLOCKS_PER_SEC*1000.0);
        // ==== end of the streamming data =======================================================


        // Timeout, milliseconds (0  - 10000, 0 --> No Timeout)
        int i_timeout_ms = 1000;

        int i_flags = ME_IO_SINGLE_TYPE_NO_FLAGS;
        //i_flags|= ME_IO_SINGLE_TYPE_TRIG_SYNCHRONOUS;  // this is don't-care in the external trigger type
        //i_flags|= ME_IO_SINGLE_TYPE_WRITE_NONBLOCKING; // this is for running writing in the background

        meIOSingle_t io_single[16];
		for(index_subdevice = 0; index_subdevice < 16; ++index_subdevice)
		{
			if(index_subdevice==15)
				i_flags|= ME_IO_SINGLE_TYPE_TRIG_SYNCHRONOUS;
			io_single[index_subdevice].iDevice    = arrSubdevices[index_subdevice].m_iDeviceIndex;
			io_single[index_subdevice].iSubdevice = arrSubdevices[index_subdevice].m_iSubdeviceIndex;
			io_single[index_subdevice].iChannel	= index_channel;
			io_single[index_subdevice].iDir		= ME_DIR_OUTPUT;
			io_single[index_subdevice].iTimeOut	= i_timeout_ms;							// No timeout - not required for software output
			io_single[index_subdevice].iFlags	= i_flags;
		}


		// digital trigger io_single_d list setting
		meIOSingle_t io_single_d[1];
		io_single_d[0].iDevice    = arrSubdevicesDigital[index_subdevice_d].m_iDeviceIndex;
		io_single_d[0].iSubdevice = arrSubdevicesDigital[index_subdevice_d].m_iSubdeviceIndex;
		io_single_d[0].iChannel   = 0;
		io_single_d[0].iDir       = ME_DIR_INPUT;
		io_single_d[0].iValue     = 0;
		io_single_d[0].iTimeOut   = 0;   // No timeout - not required for software output
		io_single_d[0].iFlags     = ME_IO_SINGLE_TYPE_DIO_BYTE;

        while(true)
		{
            for(int idx_sample = 0; idx_sample < samples_each; ++idx_sample)
            {
                for(index_subdevice = 0; index_subdevice < 16; ++index_subdevice)
                    io_single[index_subdevice].iValue = iDataOfChannel[index_subdevice][idx_sample];

                i_me_error = meIOSingle(&io_single[0],				// Output list
                                        16,							// Number of elements in the above list
                                        ME_IO_SINGLE_NO_FLAGS	);	// Flags
                if(i_me_error == ME_ERRNO_SUCCESS){}
                else if(i_me_error == ME_ERRNO_TIMEOUT)
                {
                    printf("****    meIOSingle - Timeout    ****\n\n");
                }
                else
                {
                    printf("****    meIOSingle - Error: %d    ****\n\n", i_me_error);
                    free(iDataOfChannel);
                    free(iData);
                    goto error;
                }

                // Here is the part for awaiting the trigger
                //Sleep(0.5); // recently it is controlled by the computer.
                unsigned edge_curr = 1;
                unsigned edge_prev = 1;
                while((edge_prev == edge_curr) || (edge_prev==1)){
                    edge_prev = edge_curr;
                    i_me_error = meIOSingle(&io_single_d[0],			// Output list
                                            1,							// Number of elements in the above list
                                            ME_IO_SINGLE_NO_FLAGS	);	// Flags
                    if(i_me_error != ME_ERRNO_SUCCESS)
                    {
                        printf("****    meIOSingle - Error: %d    ****\n\n", i_me_error);
                        free(iDataOfChannel);
                        free(iData);
                        goto error;
                    }
                    edge_curr = io_single_d[0].iValue & 0x00000001; // observe the LSB
                }
            }
		}
        free(iDataOfChannel);
        free(iData);
        break;
    }
	// ---------------------------------------------------------------------

	meClose(0);
	delete[] arrSubdevices;
	delete[] arrSubdevicesDigital;
#ifndef RELEASE
	printf("Program completed successfully - Press any key to terminate\n\n");
	_getch();
#endif
	return 0;

error:
	printf("Press any key to terminate\n");
    _getch();
	meClose(0);
    delete[] arrSubdevices;
    delete[] arrSubdevicesDigital;
	return -1;
}







int utilBuildAOSub(SAOSubddevice *AOSubdevice,
                   int index_device,
                   int index_subdevice
){
    int i_me_error;

    AOSubdevice->m_iDeviceIndex = index_device;// ******* main setting ********
    AOSubdevice->m_iSubdeviceIndex = index_subdevice;// ******* main setting ********

    // Asking its number of channels
    int i_number_of_channels;
    i_me_error =  meQueryNumberChannels(index_device,					// Device index
                                        index_subdevice,				// Subdevice index,
                                        &i_number_of_channels	);		// Number of ramges returned here
    if(i_me_error != ME_ERRNO_SUCCESS)
    {
        printf("****    meQueryNumberChannels - Error: %d    ****\n\n", i_me_error);
        return -1;
    }
    AOSubdevice->m_iNumberOfChannels = i_number_of_channels;// ******* main setting ********


    // Asking its number of ranges
    int i_number_of_ranges;
    i_me_error =  meQueryNumberRanges(	index_device,					// Device index
                                        index_subdevice,				// Subdevice index,
                                        ME_UNIT_ANY,					// Unit
                                        &i_number_of_ranges	);			// Number of ramges returned here
    if(i_me_error != ME_ERRNO_SUCCESS)
    {
        printf("****    meQueryNumberRanges - Error: %d    ****\n\n", i_me_error);
        return -1;
    }
    if(i_number_of_ranges > MAX_RANGES)
        i_number_of_ranges = MAX_RANGES;
    AOSubdevice->m_iNumberOfRanges = i_number_of_ranges;// ******* main setting ********
    //
    // Asking bounding values of each range
    for(int index_range = 0; index_range < i_number_of_ranges; index_range++)
    {
        int i_unit;
        double d_phys_min;
        double d_phys_max;
        int i_digital_max;

        i_me_error = meQueryRangeInfo(	index_device,					// Device index
                                        index_subdevice,				// Subdevice index,
                                        index_range,					// Range index
                                        &i_unit,						// Unit returned here - not require
                                        &d_phys_min,					// Physical minimum returned here
                                        &d_phys_max,					// Physical maximum returned here
                                        &i_digital_max				);	// Digital maximum value returned here

        if(i_me_error != ME_ERRNO_SUCCESS)
        {
            printf("****    meQueryRangeInfo - Error: %d    ****\n\n", i_me_error);
            return -1;
        }

        AOSubdevice->m_arrRanges[index_range].m_iUnit = i_unit;
        AOSubdevice->m_arrRanges[index_range].m_dPhysicalMin = d_phys_min;
        AOSubdevice->m_arrRanges[index_range].m_dPhysicalMax = d_phys_max;
        AOSubdevice->m_arrRanges[index_range].m_iDigitalMax = i_digital_max;
    }
    // General setting for each subdevice
    AOSubdevice->m_iCurrentRange = 0;
    return 0;
}



BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            // attach to process
            // return FALSE to fail DLL load
            break;

        case DLL_PROCESS_DETACH:
            // detach from process
            break;

        case DLL_THREAD_ATTACH:
            // attach to thread
            break;

        case DLL_THREAD_DETACH:
            // detach from thread
            break;
    }
    return TRUE; // succesful
}
