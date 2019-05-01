#include "DHChachaFilter.h"

#include "cryptlib.h"
#include "chacha.h"
#include "integer.h"
#include "dh.h"
#include "osrng.h"
#include "secblock.h"
#include "sha.h"
#include "blake2.h"

namespace DHParams
{
    static CryptoPP::Integer p("0xB10B8F96A080E01DDE92DE5EAE5D54EC52C99FBCFB06A3C6"
        "9A6A9DCA52D23B616073E28675A23D189838EF1E2EE652C0"
        "13ECB4AEA906112324975C3CD49B83BFACCBDD7D90C4BD70"
        "98488E9C219A73724EFFD6FAE5644738FAA31A4FF55BCCC0"
        "A151AF5F0DC8B4BD45BF37DF365C1A65E68CFDA76D4DA708"
        "DF1FB2BC2E4A4371");

    static CryptoPP::Integer g("0xA4D1CBD5C3FD34126765A442EFB99905F8104DD258AC507F"
        "D6406CFF14266D31266FEA1E5C41564B777E690F5504F213"
        "160217B4B01B886A5E91547F9E2749F4D7FBD7D3B9A92EE1"
        "909D0D2263F80A76A6A24C087A091F531DBF0A0169B6A28A"
        "D662A4D18E73AFA32D779D5918D08BC8858F4DCEF97C2A24"
        "855E6EEB22B3B2E5");

    static CryptoPP::Integer q("0xF518AA8781A8DF278ABA4E7D64B7CB9D49462353");
}

struct DHChachaFilterPimpl
{
    CryptoPP::DH m_dh;
    CryptoPP::XChaCha20::Encryption m_cipher;
    CryptoPP::SecByteBlock m_pubKey;
    CryptoPP::SecByteBlock m_priKey;
};

DHChachaFilter::DHChachaFilter()
    : m_pPimpl{GetAllocator()->New<DHChachaFilterPimpl>()}
    , m_iv{0}
{
    m_pPimpl->m_dh.AccessGroupParameters().Initialize(DHParams::p, DHParams::q, DHParams::g);

    m_pPimpl->m_priKey.resize(m_pPimpl->m_dh.PrivateKeyLength());
    m_pPimpl->m_pubKey.resize(m_pPimpl->m_dh.PublicKeyLength());

    GenerateKeys();
}

DHChachaFilter::~DHChachaFilter()
{
    GetAllocator()->Delete(m_pPimpl);
}

bool DHChachaFilter::PreConnect(Buffer::Writer* apBuffer)
{
    return apBuffer->WriteBytes(m_pPimpl->m_pubKey.BytePtr(), m_pPimpl->m_pubKey.SizeInBytes());
}

bool DHChachaFilter::ReceiveConnect(Buffer::Reader* apBuffer)
{
    CryptoPP::SecByteBlock sharedSecret(m_pPimpl->m_dh.AgreedValueLength());
    CryptoPP::SecByteBlock pubKey(m_pPimpl->m_dh.PublicKeyLength());

    if(!apBuffer->ReadBytes((CryptoPP::byte*)pubKey, pubKey.SizeInBytes()))
        return false;

    if (!m_pPimpl->m_dh.Agree(sharedSecret, m_pPimpl->m_priKey, pubKey))
        return false;
    
    CryptoPP::SecByteBlock key(CryptoPP::SHA256::DIGESTSIZE);
    CryptoPP::SecByteBlock iv(CryptoPP::BLAKE2b::DIGESTSIZE);

    CryptoPP::SHA256().CalculateDigest(key, sharedSecret, sharedSecret.SizeInBytes());
    CryptoPP::BLAKE2b().CalculateDigest(iv, sharedSecret, sharedSecret.SizeInBytes());

    std::copy(iv.BytePtr(), iv.BytePtr() + std::size(m_iv), std::begin(m_iv));

    m_pPimpl->m_cipher.SetKeyWithIV(key.BytePtr(), key.SizeInBytes(), iv.BytePtr());

    return true;
}

bool DHChachaFilter::PreSend(Buffer::Writer* apBuffer, uint32_t aSequenceNumber)
{
    (void)apBuffer;
    (void)aSequenceNumber;

    return true;
}

bool DHChachaFilter::PostSend(uint8_t* apPayload, size_t aLength, uint32_t aSequenceNumber)
{
    // Operation is reversible
    return PreReceive(apPayload, aLength, aSequenceNumber);
}

bool DHChachaFilter::PreReceive(uint8_t* apPayload, size_t aLength, uint32_t aSequenceNumber)
{
    std::array<uint8_t, 24> iv;

    std::copy(std::begin(m_iv), std::end(m_iv), std::begin(iv));

    uint8_t* pSequenceAsBytes = (uint8_t*)& aSequenceNumber;
    std::copy(pSequenceAsBytes, pSequenceAsBytes + 4, std::begin(iv) + std::size(m_iv));

    m_pPimpl->m_cipher.Resynchronize(iv.data(), std::size(iv));
    m_pPimpl->m_cipher.ProcessData(apPayload, apPayload, aLength);

    return true;
}

void DHChachaFilter::GenerateKeys()
{
    CryptoPP::AutoSeededRandomPool rng;

    m_pPimpl->m_dh.GenerateKeyPair(rng, m_pPimpl->m_priKey, m_pPimpl->m_pubKey);
}
