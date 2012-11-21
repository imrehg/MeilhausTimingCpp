#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
//#include <assert.h>
#include "Struct_Algorithm.h"

#ifdef __cplusplus
extern "C"
{
#endif
// ******************************************************
// **                  Main Algorithm                  **
// ******************************************************
ControlPointStruct*
Parse_FileTo2DArray(FILE *f){
    // FILE *f: input-file
    // return:  output-control point data
    char  linbuf[2048], *cur, *cur2;
    ControlPointStruct *ret;
    int NumCh, MaxNumPoint, NumAttr;
    int nChOnB1, nChOnB2;
    int *nCtrlPtEach;
    int i, j, ch, point, attr;
    enum VariationShape s;
    double forSine[4];

    /* pseudo code
    parse needed info for the constructor
    ret = CreateCtrlPtStruct(NumCh, MaxNumPoint, NumAttr);
    for each Channel
        for each attribute
            if attribute is 2nd reserved
                for each point
                    if the previous attribute of this point is sinusoidal
                        parse subvalue inside this segment
                    else
                        WriteCtrlPtData(ret, ich, ipoint, iattr, parsed_data); // to transfer FILE to structure
            else
                for each point
                    WriteCtrlPtData(ret, ich, ipoint, iattr, parsed_data); // to transfer FILE to structure
    */

    // first line
    fseek ( f , 0 , SEEK_SET );
    fgets(linbuf, 2048, f); // read 2048 charactoers or til EOF/NL
    nChOnB1 = strtol(linbuf, &cur, 10); //cells[1][1] in the excel
    nChOnB2 = strtol(cur+1, &cur, 10);  //cells[1][2]
    NumAttr = strtol(cur+1, &cur, 10);  //cells[1][3]
    //assert(NumAttr == 8);
    NumCh = nChOnB1+nChOnB2;
    nCtrlPtEach = (int*) malloc(sizeof(int)*(nChOnB1+nChOnB2));
    // Here NumCh and NumAttr are prepared

    // second line
    fgets(linbuf, 2048, f); // read 2048 charactoers or til EOF/NL
    cur = linbuf-1;
    MaxNumPoint = 0;
    for(ch = 0; ch < nChOnB1; ++ch){
        nCtrlPtEach[ch] = strtol(cur+1, &cur, 10); // cells[2][ch+1] in excel
        if (MaxNumPoint < nCtrlPtEach[ch])
            MaxNumPoint = nCtrlPtEach[ch];
    }

    // surfing to the line containing Board 2 channel information
    for(i=0,j=NumAttr*nChOnB1; i!=j; ){
        if(fgetc(f)=='\n')
            ++i;
    }
    // line containing Board 2 channel information
    fgets(linbuf, 2048, f); // read 2048 charactoers or til EOF/NL
    cur = linbuf-1;
    for(ch = nChOnB1; ch < nChOnB1+nChOnB2; ++ch){
        nCtrlPtEach[ch] = strtol(cur+1, &cur, 10); // cells[2][ch+1] in excel
        if (MaxNumPoint < nCtrlPtEach[ch])
            MaxNumPoint = nCtrlPtEach[ch];
    }
    // Here MaxNumPoint is prepared

    // build the manager
    ret = CreateCtrlPtStruct(NumCh, MaxNumPoint, NumAttr, nCtrlPtEach);

    // return to line 3 and start to parse for Board 1
    fseek ( f , 0 , SEEK_SET );// back to begining
    fgets(linbuf, 2048, f); // line 1
    fgets(linbuf, 2048, f); // line 2
    for(ch = 0; ch < nChOnB1+nChOnB2; ++ch){
        // skip the channel information line of Board 2
        if(ch == nChOnB1)
            fgets(linbuf, 2048, f);
        // running over the fields in csv file
        for(attr = 0; attr < NumAttr; ++attr){
            fgets(linbuf, 2048, f);
            cur = linbuf-1;
            // the treatment of attribute is divided into two parts, the 2nd reserved row and the others.
            // for the 2nd reserved row, it is possible to have more than one double in an excel cell, i.e. Sine case
            // so we have to handle this using the following if control
            if(attr == eAttrReserve){ // the 2nd reserved row
                for(point = 0; point < nCtrlPtEach[ch]; ++point){ // point as the order of point
                    s = (VariationShape)ReadCtrlPtData(ret, ch, point, eAttrShape).d;
                    // for the problematic sinusoidal, sub values seperated by ";"
                    if(s == eShapeSine){
                        cur2 = strtok(cur+1,";");
                        for(i = 0; i < 4; ++i){
                            forSine[i] = (cur2 != NULL)? strtod(cur2,NULL) : 0.0;
                            if(cur2!=NULL)
                                cur2 = strtok(NULL,";");
                        }
                        WriteCtrlPtDataP(ret, ch, point, attr, forSine, 4/*forSine size*/);
                        //updating cur
                        ++cur;// first time required since it was ended up with a "," last time.
                        while(*cur!=',' && *cur!='0')
                            ++cur;
                    }
                    else
                        WriteCtrlPtDataD(ret, ch, point, attr, strtod(cur+1, &cur));
                }
            }
            else // the other cases
                for(point = 0; point < nCtrlPtEach[ch]; ++point) // point as the order of point
                    WriteCtrlPtDataD(ret, ch, point, attr, strtod(cur+1, &cur));
        }
    }
    // finalizing
    free(nCtrlPtEach);
    return ret;
}
int
FreeProductOf_Parse_FileTo2DArray(ControlPointStruct *c){
    DestroyCtrlPtStruct(c);
    return 0;
}
int
FindWaveformSampleNum(ControlPointStruct *c, double loop, double sample_rate){
    // find the maximum time of each channel
    // and then multiply it by sample_rate and return.
    int ch;
    double ret = 0;
    double val, val2, valInc, val2Inc, value;
    for(ch = 0; ch < c->nChannels; ++ch){
        val     = ReadCtrlPtData(c,ch,c->nCpEachCh[ch]-1,eAttrTime).d;
        val2    = ReadCtrlPtData(c,ch,c->nCpEachCh[ch]-1,eAttrInter).d;
        valInc  = ReadCtrlPtData(c,ch,c->nCpEachCh[ch]-1,eAttrTimeInc).d;
        val2Inc = ReadCtrlPtData(c,ch,c->nCpEachCh[ch]-1,eAttrInterInc).d;
        value   = val + valInc*loop  +  val2 + val2Inc*loop;
        if(value > ret)
            ret = value;
    }
    return (int)(ret*sample_rate)+1;
}

int
Build_OneChannel(ControlPointStruct *c, WaveformStruct *w, double loop, double sample_rate, int ch){
    int iCPt, iWPt;
    const CP_Data *curr;
    double dVCurr, dVPrev;     // double Value Current/Previous
    int iTCurr, iTPrev, iICurr; // integer Time Current/Previous, integer Interval Current
    int ret = 0;
    int i;

    double data;
    double slope, offset; // for linear, adiabatic
    double time_const, amp/*, offset*/, E; //for exponential
    double freq/*, amp*/;

    //for the case iCPt = 0, the initial value
    iWPt = 0;
    iCPt = 0;
    curr = util_CPS_Lookup(c,ch,iCPt,0);
    dVCurr = curr[eAttrValue].d + curr[eAttrValueInc].d*loop;
    WriteWaveformData(w, ch, iWPt++, dVCurr);

    // for regular cases
    for(iCPt = 1; iCPt < c->nCpEachCh[ch]; ++iCPt){
        curr = util_CPS_Lookup(c,ch,iCPt,0);
        iTPrev = iWPt;
        iTCurr = (int)((curr[eAttrTime].d  + curr[eAttrTimeInc].d*loop) *sample_rate);
        iICurr = (int)((curr[eAttrInter].d + curr[eAttrInterInc].d*loop)*sample_rate);
        dVPrev = dVCurr;
        dVCurr = curr[eAttrValue].d + curr[eAttrValueInc].d*loop;

        // chech previous last sample is earlier than current one
        // if so, padding the last value til (including) iTCurr.
        if(iTPrev > iTCurr + 1){
            ret = 1;
            fprintf(stderr,"Timing error for Channel %d, Control point %d\n", ch, iCPt);
            iWPt = iTCurr + 1;
        }
        else while(iWPt <= iTCurr)
            WriteWaveformData(w, ch, iWPt++, dVPrev);

        switch((int)curr[eAttrShape].d){
            case eShapeLinear:
            case eShapeLinear_dup:
                offset = dVPrev;
                slope = (dVCurr - offset)/(double)iICurr;
                data = offset;
                for(i = 1; i <= iICurr; ++i){
                    data += slope;// equivalent to: offset + slope*(double)i;
                    WriteWaveformData(w, ch, iWPt++, data);
                }
                //assert(iWPt == iTCurr + iICurr + 1);
                // resulting in Waveform[iTCurr] = dVPrev, Waveform[iTCurr+iICurr] = dVCurr;
            break;
            case eShapeAdiabatic:
                offset = 1/sqrt(dVPrev);
                slope = (1/sqrt(dVCurr)-offset)/(double)iICurr;
                data = offset;
                for(i = 1; i <= iICurr; ++i){
                    data += slope;
                    WriteWaveformData(w, ch, iWPt++, 1.0/pow(data,2));
                }
                //assert(iWPt == iTCurr + iICurr + 1);
            break;
            case eShapeExponential:
                // algorithm:
                //     A*exp(    0/tc)+c = Vprev
                //     A*exp(t_int/tc)+c = Vcurr
                //     both t_int and tc are in unit integer, and solving for A and c to draw
                time_const = curr[eAttrReserve].d;
                if(time_const==0.0){
                    ret = 1;
                    fprintf(stderr,"Dividing zero error for Channel %d, Control point %d\n", ch, iCPt);
                    for(i = 1; i <= iICurr; ++i)
                        WriteWaveformData(w, ch, iWPt++, dVCurr);
                    //assert(iWPt == iTCurr + iICurr + 1);
                }
                else{
                    time_const = (dVCurr>dVPrev)? time_const : -time_const;
                    E = exp((double)iTCurr/time_const);
                    amp = (dVCurr-dVPrev)/(E-1.0);
                    offset = (dVCurr-dVPrev*E)/(1.0-E);

                    data = amp;
                    E = exp(1.0/time_const);
                    for(i = 1; i <=iICurr; ++i){
                        data *= E;
                        WriteWaveformData(w, ch, iWPt++, data+offset);
                    }
                    //assert(iWPt == iTCurr + iICurr + 1);
                }
            break;
            case eShapeSine:
                if(dVCurr != dVPrev){
                    ret = 1;
                    fprintf(stderr, "Not match voltage value in sine shape for Channel %d, Control point %d\n", ch, iCPt);
                }
                amp  = (curr[eAttrReserve].dp)[0] + (curr[eAttrReserve].dp)[2]*loop;
                freq = (curr[eAttrReserve].dp)[1] + (curr[eAttrReserve].dp)[3]*loop;
                freq *= (2*M_PI/sample_rate); // make it an angular frequency
                /* // if the freq means cycles inside the interval
                freq = (curr[eAttrReserve].dp)[1] + (curr[eAttrReserve].dp)[3]*loop;
                freq *= (2*M_PI/iICurr)
                */
                data = 0;
                for(i = 1; i <=iICurr; ++i){
                    data += freq;
                    WriteWaveformData(w, ch, iWPt++, dVCurr+amp*sin(data));
                }
                WriteWaveformData(w, ch, iWPt-1, dVCurr); // assure it being dVCurr regardless the not closed cycle.
                //assert(iWPt == iTCurr + iICurr + 1);
            break;
        }
    }
    //assert(iWPt <= w->nSamples);

    // padding last value to the end
    dVCurr = ReadWaveformData(w, ch, iWPt-1);
    while(iWPt < w->nSamples)
        WriteWaveformData(w, ch, iWPt++, dVCurr);
    return ret;
}
int
Build_AllChannel(ControlPointStruct *c, WaveformStruct *w, double loop, double sample_rate){
    // struct's: input-the structures storing necessary data
    // int loop: input-which (ordinal number) loop is this
    // The WaveformStruct input through pointer is modified.
    int i;
    for (i = 0; i < c->nChannels; ++i)
        Build_OneChannel(c,w,loop,sample_rate,i);
    return 0;
}

int
Map_ReadFromFile(FILE *f, double **key, double **value){
    // FILE *f:        the file pointer
    // double **key:   output-the lookup key
    // double **value: output-the lookup value
    // int:            return the size of the array

    int domain_size;
    int i;
    char linebuf[1024], *cur;
    double *k, *v;

    domain_size = 0;
    fseek ( f , 0 , SEEK_SET );
    while(!feof(f)){
        fgets(linebuf,1024,f);
        ++domain_size;
    }

    *key   = k = (double*)malloc(sizeof(double)*domain_size);
    *value = v = (double*)malloc(sizeof(double)*domain_size);

    i = 0;
    fseek ( f , 0 , SEEK_SET );
    while(!feof(f)){
        fgets(linebuf,1024,f);
        v[i] = strtod(linebuf, &cur);
        k[i] = strtod(cur+1, &cur);
        ++i;
    }
    --i;
    if(v[i] == v[i-1] && k[i] == k[i-1]) // repeated reading due to the csv format issue
        --domain_size;
    //if necessary, invert array
    *key   = (double*)malloc(sizeof(double)*domain_size);
    *value = (double*)malloc(sizeof(double)*domain_size);
    for(i = 0; i < domain_size; ++i){
        (*key)[i]   = k[domain_size-1-i];
        (*value)[i] = v[domain_size-1-i];
    }
    free(k);
    free(v);

    return domain_size;
}


int
FreeProductOf_Map_ReadFromFile(double *key, double *value){
    free(key);
    free(value);
    return 0;
}

int
Map_LookupPowerToVoltage(double *mapKey, double *mapValue, int size, int ch, WaveformStruct *w){
    // double *mapKey:  input-key of the map
    // double *mapValue:   input-value of the map
    // int Ch:            the channel to be mapped
    // WaveformStruct* w: inout-modified wave as output

    int     i, iMap;
    double  dWaveCur, dWavePre;
    double *slope, *wavedata;
#ifdef TIMING_EXE
    FILE *f = fopen(ch==8?"8.txt":"9.txt","w");
#endif

    // implement the slope for first order hold, to save time
    // the zeroth component is not accessible
    slope = (double*)malloc(sizeof(double)*size);
    for(iMap = 1; iMap < size; ++iMap){
        slope[iMap] = (mapValue[iMap]-mapValue[iMap-1])/(mapKey[iMap]-mapKey[iMap-1]);
#ifdef TIMING_EXE
        printf("slope[%d] = %.7f\n",iMap,slope[iMap]);
#endif
    }
    iMap = 1;
    wavedata = util_WFS_Lookup(w, ch, 0);
    dWaveCur = *wavedata - 1;// just make it diff, not to go to the onset of if control initially
    for(i = 0; i < w->nSamples; ++i){
        dWavePre = dWaveCur;
        dWaveCur = *wavedata;
#ifdef TIMING_EXE
        fprintf(f,"%.7f, %.7f\n",dWavePre, dWaveCur);
#endif
        if(dWavePre==dWaveCur){
            *wavedata = *(wavedata-1);
            ++wavedata;
        }
        else{
            while(dWaveCur>mapKey[iMap] && iMap < size)
                ++iMap;
            while(dWaveCur<mapKey[iMap-1] && iMap > 0)
                --iMap;
            if(iMap == 0)
                *(wavedata++) = mapValue[iMap++]; // ensure iMap being within 1~(size-1)
            else if(iMap == size)
                *(wavedata++) = mapValue[--iMap]; // ensure iMap being within 1~(size-1)
            else{
                *(wavedata++) = mapValue[iMap-1] + slope[iMap]*(dWaveCur-mapKey[iMap-1]);
            }
        }
    }
#ifdef TIMING_EXE
    fclose(f);
#endif
    return 0;
}
#ifdef __cplusplus
}
#endif
