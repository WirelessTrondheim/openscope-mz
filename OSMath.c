/************************************************************************/
/*                                                                      */
/*    Helper.cpp                                                        */
/*                                                                      */
/*    Helper functions to deal with date and time                       */
/*    and MAC and Number printing                                       */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2013, Digilent Inc.                                     */
/************************************************************************/
/*  Revision History:                                                   */
/*    2/1/2013(KeithV): Created                                         */
/************************************************************************/
#include <OpenScope.h>

bool OSAdd(uint8_t m1[], uint32_t cm1, uint8_t m2[], uint32_t cm2, uint8_t r[], uint32_t cr)
{
    uint32_t i;
    uint16_t sum = 0;
    
    // do not want to clear the result
    // because the result may be the same buffer as m1 or m2

    // add them up until we run out the result
    for(i=0; i<cr; i++)
    {
        if(i<cm1) sum += (unsigned short) m1[i];
        if(i<cm2) sum += (unsigned short) m2[i];
        r[i] = (unsigned char) (sum & 0xFF);
        sum >>= 8;
    }

    return(true);
}

bool OSMakeNeg(uint8_t m1[], uint32_t cm1)
{
    uint32_t i;
    uint8_t one = 1;

    for(i=0; i<cm1; i++) m1[i] = ~m1[i];
    OSAdd(m1, cm1, &one, 1, m1, cm1); 

    return(true);
}

bool OSUMult(uint8_t m1[], uint32_t cm1, uint8_t m2[], uint32_t cm2, uint8_t r[], uint32_t cr)
{
    uint32_t im1, im2, ir;

    // clear the result
    memset(r, 0, cr);

    if(cr < (cm1 + cm2))
        return(false);
 
    for(im2 = 0; im2 < cm2; im2++)
    {
        for(im1 = 0; im1 < cm1; im1++)
        {
            unsigned long sr = (unsigned long) m1[im1] * (unsigned long) m2[im2];

            for(ir = im1+im2; sr != 0 && ir < cr; ir++)
            {
                sr += ((unsigned long) r[ir]);
                r[ir] = sr & 0xFF;
                sr >>= 8;
            }
        }
    }

    return(true);
}

bool OSMult(int8_t m1[], uint32_t cm1, int8_t m2[], uint32_t cm2, int8_t r[], uint32_t cr)
{
    bool negative = false;
    uint8_t lm1[cm1];
    uint8_t lm2[cm2];

    if(m1[cm1-1] < 0)
    {
        negative ^= true;
        OSMakeNeg(lm1, cm1);
    }
    else
    {
        memcpy(lm1, m1, cm1);
    }

    if(m2[cm2-1] < 0)
    {
        negative ^= true;
        OSMakeNeg(lm2, cm2);
    }
    else
    {
        memcpy(lm2, m2, cm2);
    }

    OSUMult(lm1, cm1, lm2, cm2, (uint8_t *) r, cr);

    if(negative)
    {
        OSMakeNeg((uint8_t *) r, cr);
    }

    return(true);
}

// WARNING, the largest dl can be is 2^^56!! => 72,057,594,037,927,936
bool OSDivide(int8_t m1[], uint32_t cm1, int64_t d1, int8_t r[], uint32_t cr)
{
    bool negative = false;
    int i = 0;
    uint8_t lm1[cm1];

    // clear the result
    memset(r, 0, cr);

    // can't divide by zero
    if(d1 == 0)
        return(false);

    // just do int64 math if small enough
    else if(cm1 <= sizeof(int64_t))
    {
        int64_t r1 = 0;

        // sign extend if negative
        if(m1[cm1-1] < 0) memset(&r1, 0xFF, sizeof(r1));

        // put in r1 and divide as int64
        memcpy(&r1, m1, cm1);
        r1 /= d1;
           
        // return the result
        // if r1 is sign extend
        if(r1 < 0) memset(r, 0xFF, cr);

        // now put in the result
        if(sizeof(r1) < cr) cr = sizeof(r1);
        memcpy(r, &r1, cr);

        // done
        return(true);
    }

    // make positive
    else if(d1 < 0)
    {
        negative ^= true;
        d1 = -d1;
    }

    // make positive
    if(m1[cm1-1] < 0)
    {
        negative ^= true;
        OSMakeNeg((uint8_t *) lm1, cm1);
    }
    else
    {
        memcpy(lm1, m1, cm1);
    }

    // long division
    for(i = cm1 - sizeof(uint64_t); i >= 0; i--)
    {
        uint64_t m2, q2, r2;

        // get the upper bits for division
        memcpy(&m2, &lm1[i], sizeof(uint64_t));

        // add to the result
        // overflow could occur if the result won't fit in a int64_t
        q2 = m2 / d1;
        if(i < (int32_t) cr) OSAdd((uint8_t *) &r[i], cr - i, (uint8_t *) &q2, sizeof(q2), (uint8_t *) &r[i], cr - i);

        // get the remainder
        r2 = m2 % d1;

        // put it back and get ready for the next division.
        memcpy(&lm1[i], &r2, sizeof(uint64_t));
    }

    if(negative)
    {
        OSMakeNeg((uint8_t *) r, cr);
    }

    return(true);
}

char * ulltoa(uint64_t val, char * buf, uint32_t base)
{
	uint64_t	v;
	char		c;

	v = val;
	do {
		v /= base;
		buf++;
	} while(v != 0);
	*buf-- = 0;
	do {
		c = val % base;
		val /= base;
		if(c >= 10)
			c += 'A'-'0'-10;
		c += '0';
		*buf-- = c;
	} while(val != 0);
	return ++buf;
}

char * illtoa(int64_t val, char * buf, uint32_t base)
{
	char *	cp = buf;

	if(val < 0) {
		*buf++ = '-';
		val = -val;
	}
	ulltoa(val, buf, base);
	return cp;
}

int64_t GetSamples(int64_t psec, int64_t xsps, uint32_t scaleSPS)
{
    int32_t     np      = 1; 
    int64_t     samp    = 0;
    uint64_t    half    = 500000000000ull;
    uint8_t     r[2*sizeof(uint64_t)];
    uint8_t     r2[2*sizeof(uint64_t)];

    // The math.
    // samples = samples/sec * sec
    // samples = (xSamples/sec) * (Samples/xSamples)  * psec * (sec/1000000000000psec)
    // samples = (xsps / scaleSPS) * (psec / 1000000000000)
    // samples = (xsps * psec) / (scaleSPS * 1000000000000)

    // want to do positive math, for rounding
    if(psec < 0) 
    {
        psec *= -1;
        np = -1;
    }

    // xSamples/sec * psec
    OSUMult((uint8_t *) &psec, sizeof(psec), (uint8_t *) &xsps, sizeof(xsps), r, sizeof(r));

    // Division is hard because we will exceed our maximum of 2^^56, so we do it in 2 part
    // the first part is the smaller divisor so we get the biggest quotient. That we do can skip averaging
    // normally scaleSPS is 1000, sometimes 1000000, but 1000000000000 >> 1000000, so we don't have to round the first
    // divide because the second one will swamp it. We may be slightly off on rounding around the 50% mark, but close enough

    // (mSamples/sec * psec) / scaleSPS
    OSDivide((int8_t *) r, sizeof(r), scaleSPS, (int8_t *) r2, sizeof(r2));

    // add for rounding
    // ((mSamples/sec * psec) / scaleSPS) + ps/2        // for averaging
    OSAdd(r2, sizeof(r2), (uint8_t *) &half, sizeof(half), r2, sizeof(r2)); 

    // ps = 10^^12 = 1000000000000ull
    // (((mSamples/sec * psec) / scaleSPS) + ps/2) / ps        
    OSDivide((int8_t *) r2, sizeof(r2), 1000000000000ull, (int8_t *) &samp, sizeof(samp));

    // get our sign correct
    samp *= np;
    
    return(samp);
}

// max xsps = 1,000,000 ==  59.8 bits (still fits in int64_t)
// pratically we can go to uSPS or 1,000,000
int64_t GetPicoSec(int64_t samp, int64_t xsps, uint32_t scaleSPS)
{
    int32_t np = 1;
    uint8_t r[2*sizeof(uint64_t)];
    int64_t psec = 0;
    uint64_t psScaleSPS = 1000000000000ull * scaleSPS;
    uint64_t half = (uint64_t) (xsps/2);

    // The math.
    // sec = samples / (samples / sec)
    // psec = 10^^12 * samples / (samples / sec)
    // psec = (10^^12 * scaleSPS) * samples / ((scaleSPS * samples) / sec)

    // go positive
    if(samp < 0)
    {
        samp *= -1;
        np = -1;
    }

    OSUMult((uint8_t *) &samp, sizeof(samp), (uint8_t *) &psScaleSPS, sizeof(psScaleSPS), r, sizeof(r));
    OSAdd(r, sizeof(r), (uint8_t *) &half, sizeof(half), r, sizeof(r));
    OSDivide((int8_t *) r, sizeof(r), xsps, (int8_t *) &psec, sizeof(psec));

    // get our sign back
    psec *= np;

    return(psec);
}

// returns actual mSPS
uint64_t CalculatePreScalarAndPeriod(uint64_t xsps, uint32_t scaleSPS, uint32_t const pbClk, uint16_t * pPreScalar, uint32_t * pPeriod, uint32_t * pCnt)
{
    uint64_t pbXScale   = pbClk * ((uint64_t) scaleSPS);
    uint64_t tmr        = (pbXScale + (xsps/2)) / xsps;
    uint16_t preScalar  = 0;
    uint32_t preDivide  = 1;
    uint32_t cnt        = 1;

    // if we are too slow to use just the timer, also add outside counts
    // set prescalar to 256
    if(xsps < (TMRMINSPS * scaleSPS))
    {
        ASSERT(pCnt != NULL);

        tmr = (tmr + 128) / 256;
        cnt = tmr / MAXTMRPRX;

        if((tmr % MAXTMRPRX) > 0) cnt++;
        if(cnt == 0) cnt = 1;
        tmr = (tmr + cnt/2) / cnt;

        preScalar = 7;                  // divide by 256
        preDivide = 256;
    }

    else
    {
        for(preScalar=0; preScalar<9; preScalar++)
        {
            uint64_t tmrT = (tmr + (preDivide / 2)) / preDivide;

            if(tmrT <= MAXTMRPRX)     // we wil always substract 1 before putting in the PRx register
            {
                // we are done
                tmr = tmrT;
                break;
            }

            // go to next preScalar
            preDivide *= 2;
        }

        // there is no 8, but we are with a preDivide of 256
        if(preScalar == 8)
        {
            preScalar = 7;
        }

        // there is no preDivide of 128, so bump up to 256
        else if(preScalar == 7)
        {
            tmr = (tmr + 1) / 2;
            preDivide *= 2;
        }

        // error condition
        if(tmr == 0 || preScalar == 9)
        {
            pbXScale    = 0;        // cause return value to be zero
            tmr         = 1;        // we can not divide by zero
            preScalar   = 0;
        }
    }

    if(pPeriod != NULL)     *pPeriod    = ((uint32_t) tmr);
    if(pPreScalar != NULL)  *pPreScalar =  preScalar;
    if(pCnt != NULL)        *pCnt       = cnt;

    // pbXScale == 100,000,000 * scale which 1,2,4,8,16,32,64,256 all divide evenly into
    // so we only need to round the cnt and tmr values
    return(((pbXScale / preDivide) + (cnt*tmr/2)) / (cnt*tmr));
}

bool ScrollBuffer(uint16_t rgBuff[], int32_t cBuff, int32_t iNew, int32_t iCur)
{
    int32_t iStart = (iCur - iNew + cBuff) % cBuff;      // we want to do modulo math to keep us in the index range of the buffer
    int16_t rgHoldBuff[cBuff/2];                                // a scratch buffer

                                                                // we have to have a mult of 2 for the buffer size
    ASSERT((cBuff % 2) == 0);

    // nothing to do, return.
    if(iNew == iCur)
    {
        return(true);
    }

    else if(iStart >= cBuff/2)
    {
        memcpy(rgHoldBuff, rgBuff, sizeof(rgHoldBuff));
        memcpy(rgBuff, &rgBuff[cBuff/2], sizeof(rgHoldBuff));
        memcpy(&rgBuff[cBuff/2], rgHoldBuff, sizeof(rgHoldBuff));
        iStart -= cBuff/2;
    }

    memcpy(rgHoldBuff, rgBuff, iStart*sizeof(rgBuff[0]));
    memcpy(rgBuff, &rgBuff[iStart], (cBuff - iStart)*sizeof(rgBuff[0]));
    memcpy(&rgBuff[cBuff - iStart], rgHoldBuff, iStart*sizeof(rgBuff[0]));

    return(true);
}

bool CalculateBufferIndexes(BIDX * pbidx)
{
    int64_t absTrig2POI;

    // see if we are going to interleave
    pbidx->fInterleave = (pbidx->xsps >= pbidx->mHzInterleave);

    // calculate the actual msps and prescalar and period
    if(pbidx->fInterleave)
    {
        pbidx->xsps = 2 * CalculatePreScalarAndPeriod((pbidx->xsps + 1)/2, 1000, pbidx->pbClkSampTmr, &pbidx->tmrPreScalar, &pbidx->tmrPeriod, &pbidx->tmrCnt);
    }
    else
    {
        pbidx->xsps = CalculatePreScalarAndPeriod(pbidx->xsps, 1000, pbidx->pbClkSampTmr, &pbidx->tmrPreScalar, &pbidx->tmrPeriod, &pbidx->tmrCnt);
    }

    if(pbidx->xsps == 0)
    {
        return(false);
    }

    // calculate the number of samples delay from Trig to POI
    pbidx->dlTrig2POI = GetSamples(pbidx->psDelay, pbidx->xsps, 1000);

    // calculate the actual delay in picoseconds
    pbidx->psDelay = GetPicoSec(pbidx->dlTrig2POI, pbidx->xsps, 1000);

    absTrig2POI = pbidx->dlTrig2POI < 0 ? -pbidx->dlTrig2POI : pbidx->dlTrig2POI;

    // we have an upper limit on our return buffer
    if(pbidx->cBuff > pbidx->cDMABuff) pbidx->cBuff = pbidx->cDMABuff;

    // Case 1, both the Trig & POI can exist in the return buffer
    if(absTrig2POI < pbidx->cBuff)
    {
        pbidx->iPOI = pbidx->cBuff / 2;                             // assume the POI is in the center
        pbidx->iTrg = pbidx->iPOI - (int32_t) pbidx->dlTrig2POI;    // Place the trigger off of the delta

        // if the trigger is below zero
        // shift both so trigger is at zero
        if(pbidx->iTrg < 0)
        {
            pbidx->iPOI -= pbidx->iTrg;
            pbidx->iTrg = 0;
        }

        // if the trigger is off the top of the scale
        // shift both so trigger is the last in the buffer, or cBuff-1
        else if(pbidx->iTrg >= pbidx->cBuff)
        {
            int32_t s = pbidx->iTrg - (pbidx->cBuff - 1);   // the amount to shift

            // adjust pointers down to keep trigger in range
            pbidx->iTrg -= s;
            pbidx->iPOI -= s;
        }
    }

    // case 2, We can get the POI, that is, the POI is positive delta off of the trigger
    // but we know we can't put both trigger and POI in the buffer.
    else if(pbidx->dlTrig2POI > 0)
    {
        pbidx->iTrg = -1;
        pbidx->iPOI = pbidx->cBuff / 2;
    }

    // POI is too far negative, all we can get is the trigger.
    else
    {
        pbidx->iTrg = pbidx->cBuff - 1;
        pbidx->iPOI = -1;
    }

    // Now we have to know how to scroll the DMA buffer to align with out result pointers.
    // if the POI is completely out of the buffer, than we know that the trigger 
    // is at the last point in our return buffer
    if(pbidx->iPOI == -1) pbidx->iTrigDMA = pbidx->cBuff - 1;

    // if the trigger is before the POI (the delta will be positive), subtract a modulo of the delta from the POI to see where to scroll the trigger
    else if(pbidx->dlTrig2POI > 0) pbidx->iTrigDMA = (pbidx->iPOI + pbidx->cDMA - (absTrig2POI % pbidx->cDMA)) % pbidx->cDMA;

    // otherwise the trigger is after the POI, find the modulo to scroll too
    else pbidx->iTrigDMA = (pbidx->iPOI + absTrig2POI) % pbidx->cDMA;

    ASSERT(pbidx->iTrg == -1 || pbidx->iTrg == pbidx->iTrigDMA);

    // must take samples up to the trigger, then a few extras
    // if iTrig is -1, the trig is before our buffer and we don't any pre samples
    // but we always take half a slop, and maybe less 1 as iTrig is -1.
    pbidx->cBeforeTrig = pbidx->iTrg + pbidx->cDMASlop/2;

    // we can't go negative on timer ticks
    pbidx->cDelayTmr = GetSamples(pbidx->psDelay, TMRPBCLK*1000ll, 1000) + GetSamples(GetPicoSec(((pbidx->cBuff - pbidx->iPOI) + pbidx->cDMASlop/2), pbidx->xsps, 1000), TMRPBCLK*1000ll, 1000);
    if(pbidx->cDelayTmr <= 0) pbidx->cDelayTmr = 1;

    return(true);
}

char * GetPercent(int32_t diff, int32_t ideal, int32_t cbD, char * pchOut, int32_t cbOut)
{
    char szR[32];
    char * pchS = szR;
    char * pCur = pchOut;
    int32_t cbA = 0;
    int32_t cbB = 0;

    // this will have 7 digits after the decimal point
    int64_t diffp = ((1000000000ll * diff) + ideal/2) / ideal;

    illtoa(diffp, szR, 10);

    pchOut[cbOut-1] = '\0';
    cbOut--;

    if(szR[0] == '-' && cbOut > 0) 
    {
        pchS++; 
        *pCur = '-';
        cbOut--;
        pCur++;
    }

    cbA = strlen(pchS);

    if(cbA > 7)
    {
        cbB = cbA - 7;
        cbA = 7;
    }

    if(cbOut > 0 && cbB == 0)
    {
        *pCur = '0';
        cbOut--;
        pCur++;
    }

    while(cbB > 0 && cbOut > 0)
    {
        *pCur = *pchS;
        cbOut--;
        cbB--;
        pCur++;
        pchS++;
    }

    if(cbOut > 0)
    {
        *pCur = '.';
        cbOut--;
        pCur++;
    }

    cbB = 7 - cbA;
    if(cbA > cbD) cbA = cbD;

    while(cbB > 0 && cbOut > 0)
    {
        *pCur = '0';
        cbOut--;
        cbB--;
        pCur++;
    }

    while(cbA > 0 && cbOut > 0)
    {
        *pCur = *pchS;
        cbOut--;
        cbA--;
        pCur++;
        pchS++;
    }

    if(cbOut > 0)
    {
        *pCur = '\0';
        cbOut--;
    }

    if(*pCur == '\0') pCur--;

    while(pCur > pchOut && *pCur == '0')
    {
        *pCur = '\0';
        pCur--;
    }

	if(pCur > pchOut && *pCur == '.')
    {
        *pCur = '\0';
        pCur--;
    }

    return(pchOut);
}
