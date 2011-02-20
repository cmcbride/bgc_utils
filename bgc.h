#pragma once
#ifndef BGC_HEADER_DEFINED
#define BGC_HEADER_DEFINED 1

typedef struct group_output_header {
    int num_files;              /* number of files output is distributed into */
    int file_id;                /* this files ID (number) if multiple files are output */

    int format;                 /* output particle data format identifier, see enum pdata_format below */

    int first_group_id;         /* ID of the first group stored in this file */

    int ngroups;                /* number of groups stored LOCALLY in this file */
    int ngroups_total;          /* number of groups stored GLOBALLY over all output BGC files */

    int min_group_part;

    /*  unsigned int should be consistent with integer format unless it wraps over */
    unsigned int npart;         /* number of particles bound in groups LOCALLY in this file */
    unsigned int npart_total;   /* number of particles bound in groups GLOBALLY over all output BGC files */
    unsigned int npart_orig;    /* total number of particles of input  */

    unsigned int max_npart;     /* maximum number of particles in one group LOCALLY in this file */
    unsigned int max_npart_total;       /* maximum number of particles in one group GLOBALLY over all output BGC files */

    double linkinglength;

    double time;                /* time of the input snapshot */
    double redshift;            /* redshift of the input snapshot */

    double Omega0;
    double OmegaLambda;

    double BoxSize;

    /* these define the units, irrelevant unless binding energies are calculated */
    double Hubble0;             /* in whatever units are used */
    double GravConst;           /* in whatever units are used */

    double part_mass;           /* mass of particles if only one type exists */

    int valid_part_ids;         /* keep track of whether we read in Gadget IDs or ignored them (for Roman's lightcone) */

    char fill[904 - sizeof( char * )];  /* fill to 1024 bytes */

} OUTPUT_HEADER;

#define OUTPUT_HEADER_SIZE 1024

enum pdata_format {
    PDATA_FORMAT_ID = 10,
    PDATA_FORMAT_IDBE = 15,
    PDATA_FORMAT_POS = 20,
    PDATA_FORMAT_POSBE = 25,
    PDATA_FORMAT_PV = 30,
    PDATA_FORMAT_PVBE = 35,
    PDATA_FORMAT_PVM = 40,
    PDATA_FORMAT_PVMBE = 45,
    PDATA_FORMAT_GPVM = 50
};

static inline int
bgc_format_includes_be( int format_id )
{
    /* this MUST be kept in sync with the above enum defining the formats */
    return ( format_id % 10 ) == 5;
}

typedef struct {
    unsigned int part_id;
} PARTICLE_DATA_ID;

typedef struct {
    unsigned int part_id;
    float binding_energy;
} PARTICLE_DATA_IDBE;

typedef struct {
    unsigned int part_id;
    float pos[3];
} PARTICLE_DATA_POS;

typedef struct {
    unsigned int part_id;
    float pos[3];
    float binding_energy;
} PARTICLE_DATA_POSBE;

typedef struct {
    unsigned int part_id;
    float pos[3];
    float vel[3];
} PARTICLE_DATA_PV;

typedef struct {
    unsigned int part_id;
    float pos[3];
    float vel[3];
    float binding_energy;
} PARTICLE_DATA_PVBE;

typedef struct {
    unsigned int part_id;
    float pos[3];
    float vel[3];
    float mass;
} PARTICLE_DATA_PVM;

typedef struct {
    unsigned int part_id;
    float pos[3];
    float vel[3];
    float mass;
    float binding_energy;
} PARTICLE_DATA_PVMBE;

typedef struct {
    unsigned int part_id;
    unsigned int group_id;
    float pos[3];
    float vel[3];
    float mass;
} PARTICLE_DATA_GPVM;

/* Get the size of the PARTICLE DATA (pdata) structure based on FORMAT number */
static inline size_t
bgc_sizeof_pdata( const int pdata_format )
{
    size_t size;

    switch ( pdata_format ) {
    case PDATA_FORMAT_ID:
        size = sizeof( PARTICLE_DATA_ID );
        break;
    case PDATA_FORMAT_IDBE:
        size = sizeof( PARTICLE_DATA_IDBE );
        break;
    case PDATA_FORMAT_POS:
        size = sizeof( PARTICLE_DATA_POS );
        break;
    case PDATA_FORMAT_POSBE:
        size = sizeof( PARTICLE_DATA_POSBE );
        break;
    case PDATA_FORMAT_PV:
        size = sizeof( PARTICLE_DATA_PV );
        break;
    case PDATA_FORMAT_PVBE:
        size = sizeof( PARTICLE_DATA_PVBE );
        break;
    case PDATA_FORMAT_PVM:
        size = sizeof( PARTICLE_DATA_PVM );
        break;
    case PDATA_FORMAT_PVMBE:
        size = sizeof( PARTICLE_DATA_PVMBE );
        break;
    case PDATA_FORMAT_GPVM:
        size = sizeof( PARTICLE_DATA_GPVM );
        break;
    default:
        fprintf( stderr, "ERROR: unknown particle data format!  (format = %d)\n", pdata_format );
        size = 0;
        break;
    }
    return size;
}

#endif

// vim: ts=8 sw=4 sts=4 expandtab
