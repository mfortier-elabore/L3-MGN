#ifndef D_tddBuildTime_H
#define D_tddBuildTime_H

///////////////////////////////////////////////////////////////////////////////
//
//  tddBuildTime is responsible for recording and reporting when
//  this project library was built
//
///////////////////////////////////////////////////////////////////////////////

class tddBuildTime
  {
  public:
    explicit tddBuildTime();
    virtual ~tddBuildTime();
    
    const char* GetDateTime();

  private:
      
    const char* dateTime;

    tddBuildTime(const tddBuildTime&);
    tddBuildTime& operator=(const tddBuildTime&);

  };

#endif  // D_tddBuildTime_H
