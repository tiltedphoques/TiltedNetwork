#include "ChachaFilter.h"

#include "cryptlib.h"
#include "chacha.h"

ChachaFilter::ChachaFilter()
{
    // Note Encryption == Decryption
    m_pPimpl = GetAllocator()->New<CryptoPP::XChaCha20::Encryption>();
}

ChachaFilter::~ChachaFilter()
{

}

void ChachaFilter::ProcessTryConnect(Buffer::Writer* apBuffer)
{

}

void ChachaFilter::ProcessConnect(Buffer::Reader* apBuffer)
{
    //pCipther->SetKeyWithIV(m_key.data(), std::size(m_key), iv.data(), std::size(iv));
}

void ChachaFilter::ProcessSend(Buffer::Writer* apBuffer, uint32_t aSequenceNumber)
{
    std::array<uint8_t, 24> iv;

    std::copy(std::begin(m_iv), std::end(m_iv), std::begin(iv));

    uint8_t* pSequenceAsBytes = (uint8_t*)& aSequenceNumber;
    std::copy(pSequenceAsBytes, pSequenceAsBytes + 4, std::begin(iv) + std::size(m_iv));

    auto pCipther = (CryptoPP::XChaCha20::Encryption*)m_pPimpl;

    pCipther->Resynchronize(iv.data(), sizeof(iv));
   
    //pCipther->ProcessData(apBuffer->GetWriteData(), apBuffer->GetData(), apBuffer->GetSize());
}

void ChachaFilter::ProcessReceive(Buffer::Reader* apBuffer, uint32_t aSequenceNumber)
{
    std::array<uint8_t, 24> iv;

    std::copy(std::begin(m_iv), std::end(m_iv), std::begin(iv));

    uint8_t* pSequenceAsBytes = (uint8_t*)& aSequenceNumber;
    std::copy(pSequenceAsBytes, pSequenceAsBytes + 4, std::begin(iv) + std::size(m_iv));

    auto pCipther = (CryptoPP::XChaCha20::Encryption*)m_pPimpl;

    pCipther->Resynchronize(iv.data(), sizeof(iv));
    
    //pCipther->ProcessData(apBuffer->GetWriteData(), apBuffer->GetData(), apBuffer->GetSize());
}