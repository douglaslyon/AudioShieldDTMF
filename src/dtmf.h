
#ifndef DTMF_H
#define DTMF_H



class DTMF{
  public:
    DTMF();
    void generate(char key, unsigned long duration);
    static void generateOutput();
    
  private:
    void waveGenerator();
    void disableTimer1();
    void initTimers0();
    
  private:
    static unsigned char dtmfRowPos;
    static unsigned char dtmfColPos;

    static unsigned char rowOffset;
    static unsigned char colOffset;
};


#endif
