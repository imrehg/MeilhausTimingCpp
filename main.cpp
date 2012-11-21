// Con_meIOSingleWriteAnalogSyncExtTrigger.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <memory.h>

extern "C"
{
#include "medriver.h"
}
#include "Struct_Algorithm.h"
#include "TestConsoleGeneric.h"

int Timing(
    // intputs
    double  loop,
    double  sample_rate,
    char    *conf_path,
    // outputs
    int     *samples_each,
    int     *channels_num,
    double  **ret
){
    FILE     *f;
    int      nSample, nMap;
    double   *key, *value;
    int      i, index_max;
    double   *ret;

    ControlPointStruct  *c;
    WaveformStruct      *w;

    // parsing timing data
    f = fopen(conf_path, "r");
    if(f == NULL){
        printf("unknown path: %s", conf_path);
        return -1;
    }
    c = Parse_FileTo2DArray(f);
    fclose(f);

    // build waveform
    nSample = FindWaveformSampleNum(c, loop, sample_rate);
    w = CreateWaveformStruct(c->nChannels, nSample);
    Build_AllChannel(c,w,loop,sample_rate);
/*
    int      ch;
    // further mapping the waveform
    f = fopen(YAG1_path,"r");
    if(f == NULL){
        printf("unknown path: %s", conf_path);
        return -1;
    }
    ch = 9;
    nMap = Map_ReadFromFile(f, &key, &value);
    fclose(f);
    Map_LookupPowerToVoltage(key,value,nMap,ch,w);
    FreeProductOf_Map_ReadFromFile(key,value);

    f = fopen(YAG2_path,"r");
    if(f == NULL){
        printf("unknown path: %s", conf_path);
        return -1;
    }
    ch = 10;
    nMap = Map_ReadFromFile(f, &key, &value);
    fclose(f);
    Map_LookupPowerToVoltage(key,value,nMap,ch,w);
    FreeProductOf_Map_ReadFromFile(key,value);


    // print out the outcome
    f = fopen("CtrlPtRead.txt","w");
    display(f, c);
    fclose(f);
    for(ch = 0; ch < w->nChannels; ++ch){
        sprintf(filename,"ch%d.csv",ch);
        f = fopen(filename,"w");
        waveformRecord(f,w,ch);
        fclose(f);
    }
*/

    // putting the data
    *samples_each = w->nSamples;
    *channels_num = w->nChannels;
    *ret = w->Data;
    w->Data = NULL;

    // clearing the data
    FreeProductOf_Parse_FileTo2DArray(c);
    DestroyWaveformStruct(w);
    return 0;
}



int main(int argc, char* argv[])
{
	// Call generic console function in parent directory to
	// open system and list the available devices and subdevices.

	int i_number_of_devices = TestConsoleMEIDSProlog();

	if(i_number_of_devices <= 0)
		return(-1);


	// ---------------------------------------------------------------------

	// Count the AO sub-devices in the system

	int i_number_ao_subdevices = 16;

	int i_me_error;

	// Make a list of the AO sub-devices

	struct SAORange
	{
		int		m_iUnit;

		double	m_dPhysicalMin;
		double	m_dPhysicalMax;

		int		m_iDigitalMax;
	};

#define MAX_RANGES	 16

	struct SAOSubddevice
	{
		int	m_iDeviceIndex;
		int m_iSubdeviceIndex;
		int m_iNumberOfChannels;
		int m_iNumberOfRanges;
		SAORange m_arrRanges[MAX_RANGES];
		int m_iCurrentRange;
	};

	SAOSubddevice* arrSubdevices = new SAOSubddevice[i_number_ao_subdevices];

	i_number_ao_subdevices = 0;

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
		i_me_error = meQueryNumberSubdevices(	index_device,				// Device index
												&i_number_of_subdevices);	// Number of subdevices returned here

        // The while-loop building the subdevice structure for each subdevice
		while(true)
		{
			i_me_error = meQuerySubdeviceByType(index_device,					// Device
												index_subdevice,				// Begin search at this sub-device
												ME_TYPE_AO,						// Type of sub-device to search for
												ME_SUBTYPE_ANY,					// Sub-type of sub-device to search for
												&index_subdevice	);			// sub-device index returned here

			if(i_me_error != ME_ERRNO_SUCCESS)
				break;
			arrSubdevices[i_number_ao_subdevices].m_iDeviceIndex = index_device;
			arrSubdevices[i_number_ao_subdevices].m_iSubdeviceIndex = index_subdevice;


            // Asking its number of channels
			int i_number_of_channels;
			i_me_error =  meQueryNumberChannels(index_device,					// Device index
												index_subdevice,				// Subdevice index,
												&i_number_of_channels	);			// Number of ramges returned here
			if(i_me_error != ME_ERRNO_SUCCESS)
			{
				printf("****    meQueryNumberChannels - Error: %d    ****\n\n", i_me_error);
                goto error;
			}
			arrSubdevices[i_number_ao_subdevices].m_iNumberOfChannels = i_number_of_channels;


            // Asking its number of ranges
			int i_number_of_ranges;
			i_me_error =  meQueryNumberRanges(	index_device,					// Device index
												index_subdevice,				// Subdevice index,
												ME_UNIT_ANY,					// Unit
												&i_number_of_ranges	);			// Number of ramges returned here
			if(i_me_error != ME_ERRNO_SUCCESS)
			{
				printf("****    meQueryNumberRanges - Error: %d    ****\n\n", i_me_error);
                goto error;
			}
			if(i_number_of_ranges > MAX_RANGES)
				i_number_of_ranges = MAX_RANGES;
			arrSubdevices[i_number_ao_subdevices].m_iNumberOfRanges = i_number_of_ranges;
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
												&i_unit,							// Unit returned here - not require
												&d_phys_min,					// Physical minimum returned here
												&d_phys_max,					// Physical maximum returned here
												&i_digital_max				);	// Digital maximum value returned here

				if(i_me_error != ME_ERRNO_SUCCESS)
				{
					printf("****    meQueryRangeInfo - Error: %d    ****\n\n", i_me_error);
					goto error;
				}

				arrSubdevices[i_number_ao_subdevices].m_arrRanges[index_range].m_iUnit = i_unit;
				arrSubdevices[i_number_ao_subdevices].m_arrRanges[index_range].m_dPhysicalMin = d_phys_min;
				arrSubdevices[i_number_ao_subdevices].m_arrRanges[index_range].m_dPhysicalMax = d_phys_max;
				arrSubdevices[i_number_ao_subdevices].m_arrRanges[index_range].m_iDigitalMax = i_digital_max;
			}


            // General setting for each subdevice
			arrSubdevices[i_number_ao_subdevices].m_iCurrentRange = 0;
            ++i_number_ao_subdevices;
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
//// ******************The End of Acquiring the subdevices information********************* ////
//// ************The following is the configuration and the main output part.************** ////

	while(true)
	{
        // this should be iterated
		int index_subdevice = 0;

		// there is just one channel in analog output
		int index_channel = 0;

		// there is only one range
		int index_range = 0;

		// I choose synchronous mode
		int i_trigger_channel = ME_TRIG_CHAN_SYNCHRONOUS;
		//i_trigger_channel = ME_TRIG_CHAN_DEFAULT;
		//i_trigger_channel = ME_TRIG_CHAN_SYNCHRONOUS;

		// I choose external digital trigger
		int i_trigger_type = ME_TRIG_TYPE_SW;
		//i_trigger_type = ME_TRIG_TYPE_SW;
		//i_trigger_type = ME_TRIG_TYPE_EXT_DIGITAL;

		int i_trigger_edge = ME_TRIG_EDGE_RISING;
		//i_trigger_edge = ME_VALUE_NOT_USED;
		//i_trigger_edge = ME_TRIG_EDGE_RISING;
		//i_trigger_edge = ME_TRIG_EDGE_FALLING;
		//i_trigger_edge = ME_TRIG_EDGE_ANY;

		// Configure the device
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

			if(i_me_error == ME_ERRNO_SUCCESS)
			{
				printf("\nsub-device configured\n\n");
				arrSubdevices[index_subdevice].m_iCurrentRange = index_range;
			}
			else
			{
				printf("\n****    meIOSingleConfig - Error: %d    ****\n\n", i_me_error);
				goto error;
			}
		}

		// Fetch the range information which will be required to convert from
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
        printf("max:%.5fV,  min:%.5fV\n",d_phys_max,d_phys_min);
		if(i_me_error != ME_ERRNO_SUCCESS)
		{
			printf("****    meQueryRangeInfo - Error: %d    ****\n\n", i_me_error);
			goto error;
		}

		// ==== This is the Testing Streamming data ======================================================
        int i_digital_value, myError;
		double d_value;
        double  loop = 0;
        double  sample_rate = 20
        char    *conf_path = ""
        // outputs
        int     samples_each;
        int     channels_num;
        double  *ret
        myError =  Timing(
            // intputs
            loop,  sample_rate, conf_path,
            // outputs
            &samples_each, &channels_num,&ret
        )
		double d_stream_data[18] = {-1.0, -2.0, -3.0, -4.0, -5.0, -6.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 0.0, 2.0, -2.0, 0.0, 1.0, 2.0};
		int    i_stream_data[18];
		int    i_stream_data2[18];
		int idx_stream_data = 0;
		// normalize the streamming data into the range of d_phys_min..d_phys_max
		for(idx_stream_data = 0; idx_stream_data < 18; ++idx_stream_data)
		{
			d_value = d_stream_data[idx_stream_data];
			if(d_value > d_phys_max)
				d_value = d_phys_max;
			else if (d_value < d_phys_min)
				d_value = d_phys_min;
			d_stream_data[idx_stream_data] = d_value;
			i_me_error = meUtilityPhysicalToDigital(d_phys_min,				// Minimum physiacal value
													d_phys_max,				// Maximum physical value
													i_digital_max,			// Maximum digital value
													d_value,				// Physical value to convert
													&i_digital_value	);	// Corresponding digital value for meIOSingle returned here
			if(i_me_error != ME_ERRNO_SUCCESS)
			{
				printf("****    meUtilityPhysicalToDigital - Error: %d    ****\n\n", i_me_error);
				goto error;
			}
			i_stream_data[idx_stream_data] = i_digital_value;
		}
		// for trigger signal
		i_me_error = meUtilityPhysicalToDigital(d_phys_min,				// Minimum physiacal value
                                                d_phys_max,				// Maximum physical value
                                                i_digital_max,			// Maximum digital value
                                                0.0,				// Physical value to convert
                                                &i_digital_value	);	// Corresponding digital value for meIOSingle returned here
        i_stream_data2[0] = i_digital_value;
        i_me_error = meUtilityPhysicalToDigital(d_phys_min,				// Minimum physiacal value
                                                d_phys_max,				// Maximum physical value
                                                i_digital_max,			// Maximum digital value
                                                5.0,				// Physical value to convert
                                                &i_digital_value	);	// Corresponding digital value for meIOSingle returned here
        i_stream_data2[1] = i_digital_value;
        for(int i = 2; i < 10; ++i)
            i_stream_data2[i] = i_stream_data2[1];
        for(int i = 10; i < 18; ++i)
            i_stream_data2[i] = i_stream_data2[0];
		// end of the testing streamming data


		// Timeout, milliseconds (0  - 10000, 0 --> No Timeout)
		int i_timeout_ms = 1000;

		int i_flags = ME_IO_SINGLE_TYPE_NO_FLAGS;
		//i_flags|= ME_IO_SINGLE_TYPE_TRIG_SYNCHRONOUS;  // this is don't-care in the external trigger type
		//i_flags|= ME_IO_SINGLE_TYPE_WRITE_NONBLOCKING; // this is for running writing in the background

		meIOSingle_t io_single[16];
		idx_stream_data = 0;
		while(true)
		{
			for(index_subdevice = 0; index_subdevice < 16; ++index_subdevice)
			{
			    i_flags = ME_IO_SINGLE_TYPE_NO_FLAGS;
				//if(index_subdevice==15)
					i_flags|= ME_IO_SINGLE_TYPE_TRIG_SYNCHRONOUS;

                if(index_subdevice==1)
                    i_digital_value = i_stream_data2[idx_stream_data];
                else
                    i_digital_value = i_stream_data[idx_stream_data];

				io_single[index_subdevice].iDevice    = arrSubdevices[index_subdevice].m_iDeviceIndex;
				io_single[index_subdevice].iSubdevice = arrSubdevices[index_subdevice].m_iSubdeviceIndex;
				io_single[index_subdevice].iChannel	= index_channel;
				io_single[index_subdevice].iDir		= ME_DIR_OUTPUT;
				io_single[index_subdevice].iValue	= i_digital_value;
				io_single[index_subdevice].iTimeOut	= i_timeout_ms;							// No timeout - not required for software output
				io_single[index_subdevice].iFlags	= i_flags;
			}
			i_me_error = meIOSingle(&io_single[0],				// Output list
									16,							// Number of elements in the above list
									ME_IO_SINGLE_NO_FLAGS	);	// Flags
			++idx_stream_data;
			if(idx_stream_data==18)
                idx_stream_data=0;
			if(i_me_error == ME_ERRNO_SUCCESS)
			{
/*
				printf(	"Output %5.2lf V (%5d) to Device: %2d  Sub-device: %2d\n\n\n",
						d_value,
						i_digital_value,
						arrSubdevices[index_subdevice].m_iDeviceIndex,
						arrSubdevices[index_subdevice].m_iSubdeviceIndex					);
*/
			}
			else if(i_me_error == ME_ERRNO_TIMEOUT)
				printf("****    meIOSingle - Timeout    ****\n\n");
			else
			{
				printf("****    meIOSingle - Error: %d    ****\n\n", i_me_error);
				goto error;
			}
            // Here is the part for awaiting the trigger
            // recently it is controlled by the computer.
			Sleep(0.5);
		}
	}

	// ---------------------------------------------------------------------

	meClose(0);

	printf("Program completed successfully - Press any key to terminate\n\n");

	_getch();

	return 0;

error:;

	printf("Press any key to terminate\n");

    _getch();

	meClose(0);

	return -1;
}

