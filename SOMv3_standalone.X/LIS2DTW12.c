#include "LIS2DTW12.h"

void LIS2DTW12_Create(void)
{
    // Configuration de l'accelerometre
    I2CHelper_WriteRegister(LIS2DTW12_ADDR, &LIS2DTW12_CTRL1, 0x00);
    
    /*
     Option 1 : on-demand. Ecrit 1 dans SLP_MODE, puis il y a un interrupt (INT1)
     * quand les données sont disponibles (DRDY). Après la lecture des données,
     * il retourne en sleep. Au prochain trig il y aura un delai de wake up
     * qui varie de 1.2 à 3.55ms (low power modes 1-4)
     * 
     * Le mode low noise consomme un peu plus, mais ce n'est pas critique pour
     * cette application. (MODE et LP_MODE dans CTRL1)
     * 
     * Les valeurs de calibration sont appliquées automatiquement.
     * 
     * Self-test possible pour faire varier les signaux sur toute la plage.
     * 
     * FIFO : à mettre en mode bypass
     *  FIFO_CTRL (0x2E) - FMode = 000
     * 
     * 
     */
}

void LIS2DTW12_Destroy(void)
{
}


