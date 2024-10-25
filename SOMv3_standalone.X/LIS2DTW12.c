#include "LIS2DTW12.h"

void LIS2DTW12_updateMovingAverage(uint16_t * val, uint16_t value, float alpha);

struct Accel_t accel;

void LIS2DTW12_Create(void)
{
    accel.fastMovingAverage = 0;
    accel.slowMovingAverage = 16000;    // 1g
    accel.filteredValue = 0;
    accel.lastValue = 0;
    
    // BOOT -> recupere les donnees de calibration (si perdues)
    // RST les registres aux valeurs par défaut
    uint8_t data = 0xC0; // 11000000 = 0x80
    I2CHelper_WriteRegister(LIS2DTW12_ADDR, &LIS2DTW12_CTRL2, &data);
    
    /* Configuration de l'accelerometre
     * 0b01010000 & 0b11110000 : ODR = 0101 (100 Hz) -> 25Hz, 0001
     * 0b0100 & 0b1100 : Mode = 01 (high performance)
     * LP Mode selection : 00 = inutilisé
     * Mot de controle : 0b0101-01-00 = 0x54 -> 00010100 = 0x14
     */
    data = 0x14;
    I2CHelper_WriteRegister(LIS2DTW12_ADDR, &LIS2DTW12_CTRL1, &data);
    
    // Self test
    //data = 0x80; // 10000000 = 0x80
    //I2CHelper_WriteRegister(LIS2DTW12_ADDR, &LIS2DTW12_CTRL3, &data);
    
    
    /* Configuration de la bande passante et des filtres
     * 0b00 : Bande passante ODR/2
     * 0b00 : FS - +/-2g
     * 0b1 : Low Noise (enabled)
     * 0b0000100
     */
    data = 0x04;
    I2CHelper_WriteRegister(LIS2DTW12_ADDR, &LIS2DTW12_CTRL6, &data);
    
    /*
     * Les valeurs de calibration sont appliquées automatiquement.
     * 
     * Self-test possible pour faire varier les signaux sur toute la plage.
     * 
     * FIFO : à mettre en mode bypass
     *  FIFO_CTRL (0x2E) - FMode = 
     * 0b110
     * 0b11111
     *  = 0xC0
     */
    data = 0x00;
    I2CHelper_WriteRegister(LIS2DTW12_ADDR, &LIS2DTW12_FIFO_CTRL, &data);
}

int16_t LIS2DTW12_ReadZAxis(void) {
    int16_t data;
    I2CHelper_ReadMultipleRegisters(LIS2DTW12_ADDR, &LIS2DTW12_OUT_Z_L, &data, 2);
    return data;
}

void LIS2DTW12_Destroy(void)
{
}

void LIS2DTW12_updateMovingAverage(uint16_t * ma, uint16_t value, float alpha) {
    *ma = (uint16_t) (alpha * value + (1 - alpha) * (*ma));
}

void LIS2DTW12_Update(void) {
    accel.lastValue = LIS2DTW12_ReadZAxis();
    
    LIS2DTW12_updateMovingAverage(&accel.slowMovingAverage, accel.lastValue, 0.2);
    
    if(accel.lastValue > accel.slowMovingAverage) {
        accel.filteredValue = accel.lastValue - accel.slowMovingAverage;
    } else {
        accel.filteredValue = accel.slowMovingAverage - accel.lastValue;
    }
    
    LIS2DTW12_updateMovingAverage(&accel.fastMovingAverage, accel.filteredValue, 0.2);
    //printf("\nval : %hu\tslow: %hu\tfast : %hu", accel.lastValue, accel.slowMovingAverage, accel.fastMovingAverage);
}

uint16_t LIS2DTW12_GetValue(void) {
    return accel.fastMovingAverage;
}