#include "MyLinearReader.h"
#include <bento4/Ap4SencAtom.h>

MyLinearReader::MyLinearReader(AP4_Movie& movie, AP4_ByteStream* fragment_stream ):AP4_LinearReader(movie, fragment_stream) {

	
}
AP4_Result MyLinearReader::ProcessMoof(AP4_ContainerAtom* moof, AP4_Position moof_offset, AP4_Position mdat_payload_offset)
{
	
	if (!this->AP4_LinearReader::ProcessMoof(moof, moof_offset, mdat_payload_offset)) {
		moof = 0;
		AP4_TfhdAtom* tfhd = 0;
		AP4_TrexAtom* trex = 0;
		this->m_Fragment->GetTrafAtom(this->m_Trackers[0]->m_Track->GetId(), moof);
		if (moof) {
			tfhd = dynamic_cast<AP4_TfhdAtom*>(moof->GetChild(AP4_ATOM_TYPE_TFHD));
			AP4_MoovAtom* m_MoovAtom = this->m_Movie.GetMoovAtom();
			AP4_ContainerAtom * mvex = dynamic_cast<AP4_ContainerAtom*>(m_MoovAtom->GetChild(AP4_ATOM_TYPE_MVEX));
			if(trex)
				trex = dynamic_cast<AP4_TrexAtom*>(mvex->GetChild(AP4_ATOM_TYPE_TREX));
		}
		int index = 0;
		if (trex)
			index = trex->GetDefaultSampleDescriptionIndex();
		if (tfhd && (tfhd->GetFlags() & 2 )!= 0) {
			index = tfhd->GetSampleDescriptionIndex();
		}
		if (index) {
			index--;
		}
	
			
		AP4_SampleDescription* sdescription = this->m_Trackers[0]->m_Track->GetSampleDescription(index);
		if (sdescription->GetType() == AP4_SampleDescription::Type::TYPE_PROTECTED) {

			AP4_ProtectedSampleDescription* ProtectedSampleDescription = dynamic_cast<AP4_ProtectedSampleDescription*>(sdescription);
			AP4_ProtectionSchemeInfo* SchemeInfo = ProtectedSampleDescription->GetSchemeInfo();
			AP4_ContainerAtom* SchiAtom = SchemeInfo->GetSchiAtom();
			AP4_CencTrackEncryption* cenc = dynamic_cast<AP4_CencTrackEncryption*>(SchiAtom->GetChild(AP4_ATOM_TYPE_TENC));
			if (!cenc) {

				//SchiAtom->GetChild()  100D8019
			}
			const AP4_UI08* kid = cenc->GetDefaultKid();
			AP4_SencAtom* SencAtom = dynamic_cast<AP4_SencAtom*>(moof->GetChild(AP4_ATOM_TYPE_SENC));
			AP4_CencSampleEncryption* CencSampleEncryption = (AP4_CencSampleEncryption*)(((char*)SencAtom) + 0x28);
			AP4_CencSampleInfoTable* table;
			if (!CencSampleEncryption || (CencSampleEncryption->GetOuter().GetFlags() & 1)==0){
				AP4_UI08 DefaultPerSampleIvSize = cenc->GetDefaultPerSampleIvSize();
				AP4_UI08 m_DefaultConstantIvSize = cenc->GetDefaultConstantIvSize();
				AP4_UI08 m_DefaultCryptByteBlock = cenc->GetDefaultCryptByteBlock();
				AP4_UI08 m_DefaultSkipByteBlock = cenc->GetDefaultSkipByteBlock();
				const AP4_UI08* m_DefaultConstantIv = 0;
				if (m_DefaultConstantIvSize) {
					m_DefaultConstantIv = cenc->GetDefaultConstantIv();
				}
				if (CencSampleEncryption) {
					CencSampleEncryption->CreateSampleInfoTable(0, m_DefaultCryptByteBlock, m_DefaultSkipByteBlock, DefaultPerSampleIvSize, m_DefaultConstantIvSize, m_DefaultConstantIv, table);
				}
			}
		


			//CencSampleEncryption->CreateSampleInfoTable(0,)
			printf("����Ƭ��\n");
		}
		else {
			printf("δ����Ƭ��\n");
		}
	}
	return AP4_Result();
}
