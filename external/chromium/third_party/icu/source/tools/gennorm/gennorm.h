
#ifndef __GENPROPS_H__
#define __GENPROPS_H__

#include "unicode/utypes.h"
#include "unicode/uset.h"

/* file definitions */
#define DATA_NAME "unorm"
#define DATA_TYPE "icu"

typedef struct Norm {
    uint8_t udataCC, lenNFD, lenNFKD;
    uint8_t qcFlags, combiningFlags;
    uint16_t canonBothCCs, compatBothCCs, combiningIndex, specialTag;
    uint32_t *nfd, *nfkd;
    uint32_t value32; /* temporary variable for generating runtime norm32 and fcd values */
    int32_t fncIndex;
    USet *canonStart;
    UBool unsafeStart;
} Norm;

enum {
    UGENNORM_STORE_COMPAT,      /* (k) compatibility decompositions */
    UGENNORM_STORE_COMPOSITION, /* (c) composition data */
    UGENNORM_STORE_FCD,         /* (f) FCD data */
    UGENNORM_STORE_AUX,         /* (a) auxiliary trie and associated data */
    UGENNORM_STORE_EXCLUSIONS,  /* (x) exclusion sets */
    UGENNORM_STORE_COUNT
};

extern uint32_t gStoreFlags;

#define DO_STORE(flag)      (0!=(gStoreFlags&U_MASK(flag)))
#define DO_NOT_STORE(flag)  (0==(gStoreFlags&U_MASK(flag)))

/* global flags */
extern UBool beVerbose, haveCopyright;

/* prototypes */
extern void
setUnicodeVersion(const char *v);

extern void
init(void);

extern void
storeNorm(uint32_t code, Norm *norm);

extern void
setQCFlags(uint32_t code, uint8_t qcFlags);

extern void
setCompositionExclusion(uint32_t code);

U_CFUNC void
setFNC(uint32_t c, UChar *s);

extern void
processData(void);

extern void
generateData(const char *dataDir, UBool csource);

extern void
cleanUpData(void);

#endif

