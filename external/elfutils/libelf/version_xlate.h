
#include <assert.h>
#include <gelf.h>

#include "libelfP.h"


static void
elf_cvt_Verdef (void *dest, const void *src, size_t len, int encode)
{
  /* We have two different record types: ElfXX_Verndef and ElfXX_Verdaux.
     To recognize them we have to walk the data structure and convert
     them one after the other.  The ENCODE parameter specifies whether
     we are encoding or decoding.  When we are encoding we can immediately
     use the data in the buffer; if not, we have to decode the data before
     using it.  */
  size_t def_offset = 0;
  GElf_Verdef *ddest;
  GElf_Verdef *dsrc;

  /* We rely on the types being all the same size.  */
  assert (sizeof (GElf_Verdef) == sizeof (Elf32_Verdef));
  assert (sizeof (GElf_Verdaux) == sizeof (Elf32_Verdaux));
  assert (sizeof (GElf_Verdef) == sizeof (Elf64_Verdef));
  assert (sizeof (GElf_Verdaux) == sizeof (Elf64_Verdaux));

  if (len == 0)
    return;

  do
    {
      size_t aux_offset;
      GElf_Verdaux *asrc;

      /* Test for correct offset.  */
      if (def_offset + sizeof (GElf_Verdef) > len)
	return;

      /* Work the tree from the first record.  */
      ddest = (GElf_Verdef *) ((char *) dest + def_offset);
      dsrc = (GElf_Verdef *) ((char *) src + def_offset);

      /* Decode first if necessary.  */
      if (! encode)
	{
	  ddest->vd_version = bswap_16 (dsrc->vd_version);
	  ddest->vd_flags = bswap_16 (dsrc->vd_flags);
	  ddest->vd_ndx = bswap_16 (dsrc->vd_ndx);
	  ddest->vd_cnt = bswap_16 (dsrc->vd_cnt);
	  ddest->vd_hash = bswap_32 (dsrc->vd_hash);
	  ddest->vd_aux = bswap_32 (dsrc->vd_aux);
	  ddest->vd_next = bswap_32 (dsrc->vd_next);

	  aux_offset = def_offset + ddest->vd_aux;
	}
      else
	aux_offset = def_offset + dsrc->vd_aux;

      /* Handle all the auxiliary records belonging to this definition.  */
      do
	{
	  GElf_Verdaux *adest;

	  /* Test for correct offset.  */
	  if (aux_offset + sizeof (GElf_Verdaux) > len)
	    return;

	  adest = (GElf_Verdaux *) ((char *) dest + aux_offset);
	  asrc = (GElf_Verdaux *) ((char *) src + aux_offset);

	  if (encode)
	    aux_offset += asrc->vda_next;

	  adest->vda_name = bswap_32 (asrc->vda_name);
	  adest->vda_next = bswap_32 (asrc->vda_next);

	  if (! encode)
	    aux_offset += adest->vda_next;
	}
      while (asrc->vda_next != 0);

      /* Encode now if necessary.  */
      if (encode)
	{
	  def_offset += dsrc->vd_next;

	  ddest->vd_version = bswap_16 (dsrc->vd_version);
	  ddest->vd_flags = bswap_16 (dsrc->vd_flags);
	  ddest->vd_ndx = bswap_16 (dsrc->vd_ndx);
	  ddest->vd_cnt = bswap_16 (dsrc->vd_cnt);
	  ddest->vd_hash = bswap_32 (dsrc->vd_hash);
	  ddest->vd_aux = bswap_32 (dsrc->vd_aux);
	  ddest->vd_next = bswap_32 (dsrc->vd_next);
	}
      else
	def_offset += ddest->vd_next;
    }
  while (dsrc->vd_next != 0);
}


static void
elf_cvt_Verneed (void *dest, const void *src, size_t len, int encode)
{
  /* We have two different record types: ElfXX_Verndef and ElfXX_Verdaux.
     To recognize them we have to walk the data structure and convert
     them one after the other.  The ENCODE parameter specifies whether
     we are encoding or decoding.  When we are encoding we can immediately
     use the data in the buffer; if not, we have to decode the data before
     using it.  */
  size_t need_offset = 0;
  GElf_Verneed *ndest;
  GElf_Verneed *nsrc;

  /* We rely on the types being all the same size.  */
  assert (sizeof (GElf_Verneed) == sizeof (Elf32_Verneed));
  assert (sizeof (GElf_Vernaux) == sizeof (Elf32_Vernaux));
  assert (sizeof (GElf_Verneed) == sizeof (Elf64_Verneed));
  assert (sizeof (GElf_Vernaux) == sizeof (Elf64_Vernaux));

  if (len == 0)
    return;

  do
    {
      size_t aux_offset;
      GElf_Vernaux *asrc;

      /* Test for correct offset.  */
      if (need_offset + sizeof (GElf_Verneed) > len)
	return;

      /* Work the tree from the first record.  */
      ndest = (GElf_Verneed *) ((char *) dest + need_offset);
      nsrc = (GElf_Verneed *) ((char *) src + need_offset);

      /* Decode first if necessary.  */
      if (! encode)
	{
	  ndest->vn_version = bswap_16 (nsrc->vn_version);
	  ndest->vn_cnt = bswap_16 (nsrc->vn_cnt);
	  ndest->vn_file = bswap_32 (nsrc->vn_file);
	  ndest->vn_aux = bswap_32 (nsrc->vn_aux);
	  ndest->vn_next = bswap_32 (nsrc->vn_next);

	  aux_offset = need_offset + ndest->vn_aux;
	}
      else
	aux_offset = need_offset + nsrc->vn_aux;

      /* Handle all the auxiliary records belonging to this requirement.  */
      do
	{
	  GElf_Vernaux *adest;

	  /* Test for correct offset.  */
	  if (aux_offset + sizeof (GElf_Vernaux) > len)
	    return;

	  adest = (GElf_Vernaux *) ((char *) dest + aux_offset);
	  asrc = (GElf_Vernaux *) ((char *) src + aux_offset);

	  if (encode)
	    aux_offset += asrc->vna_next;

	  adest->vna_hash = bswap_32 (asrc->vna_hash);
	  adest->vna_flags = bswap_16 (asrc->vna_flags);
	  adest->vna_other = bswap_16 (asrc->vna_other);
	  adest->vna_name = bswap_32 (asrc->vna_name);
	  adest->vna_next = bswap_32 (asrc->vna_next);

	  if (! encode)
	    aux_offset += adest->vna_next;
	}
      while (asrc->vna_next != 0);

      /* Encode now if necessary.  */
      if (encode)
	{
	  need_offset += nsrc->vn_next;

	  ndest->vn_version = bswap_16 (nsrc->vn_version);
	  ndest->vn_cnt = bswap_16 (nsrc->vn_cnt);
	  ndest->vn_file = bswap_32 (nsrc->vn_file);
	  ndest->vn_aux = bswap_32 (nsrc->vn_aux);
	  ndest->vn_next = bswap_32 (nsrc->vn_next);
	}
      else
	need_offset += ndest->vn_next;
    }
  while (nsrc->vn_next != 0);
}
