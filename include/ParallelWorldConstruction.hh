#ifndef ParallelWorldConstruction_hh
#define ParallelWorldConstruction_hh

#include "globals.hh"
#include "G4VUserParallelWorld.hh"

class ParallelWorldConstruction : public G4VUserParallelWorld {
public:
  ParallelWorldConstruction(G4String& worldName);
  virtual ~ParallelWorldConstruction();
  virtual void Construct();
private:
  DetectorConstruction   *theDetector ;

};

#endif
