#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <Wincrypt.h>
#pragma comment(lib, "crypt32.lib")

///
/// Error handler. When called uses GetLastError and Formatmessage() to determine why an API call failed.
void ErrorHandler(char *s)
{
	HRESULT hError = GetLastError();
	LPTSTR errorText = NULL;

	FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		hError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&errorText,
		0,
		NULL);

	fprintf(stderr, "---\n");
	fprintf(stderr, "ERROR\n");
	fprintf(stderr, "%s\n", s);
	fprintf(stderr, "Number: %x\n", GetLastError());
	if (NULL != errorText)
	{
		_ftprintf(stderr, TEXT("Message: %s\n"), errorText);
		LocalFree(errorText);
		errorText = NULL;
	}
	fprintf(stderr, "---\n");
	exit(1);
}


///
/// Disabled all enhanced key usages for the specified certificate.
///
void RemoveAllEnhancedKeyUsages(LPTSTR pszStoreName, LPTSTR szSubjectName)
{
	HANDLE hStoreHandle = NULL;
	PCCERT_CONTEXT pCertContext = NULL;


	// Open Store.
	if (hStoreHandle = CertOpenStore(CERT_STORE_PROV_SYSTEM, 0, NULL, CERT_SYSTEM_STORE_LOCAL_MACHINE, pszStoreName))
	{
		_tprintf(TEXT("[i] Successfully opened certificate store '%s'\n"), pszStoreName);
	}
	else
	{
		ErrorHandler("Failed to open certificate store");
		exit(1);
	}

	//
	// Iterate over certificates in this store.
	while (pCertContext = CertEnumCertificatesInStore(hStoreHandle, pCertContext))
	{
		DWORD cbSize;
		LPTSTR pszName;

		// Get the Certificate Subject Name.
		cbSize = CertNameToStr(pCertContext->dwCertEncodingType, &(pCertContext->pCertInfo->Subject), CERT_OID_NAME_STR, NULL, 0);
		if (1 == cbSize)
		{
			ErrorHandler("Failed to return the subject name of the certificate.");
		}
		if (!(pszName = (LPTSTR)malloc(cbSize * sizeof(TCHAR))))
		{
			ErrorHandler("Failed to allocate memory");
		}
		if (CertGetNameString(pCertContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0, NULL, pszName, cbSize))
		{
//			_tprintf(TEXT("%s\n"), pszName);
		}
		else
		{
			ErrorHandler("CertGetNameString failed");
		}

		//
		// Is this the certificate we're looking for?
		if (_tcscmp(pszName, szSubjectName) == 0)
		{
			_tprintf(TEXT("[i] Located certificate %s\n"), pszName);
			PCERT_ENHKEY_USAGE pEKeyUsage = NULL;

			if (!(pEKeyUsage = (PCERT_ENHKEY_USAGE)CryptMemAlloc(sizeof(CERT_ENHKEY_USAGE))))
			{
				ErrorHandler("Failed to allocate memory");
			}
			if (CertSetEnhancedKeyUsage(pCertContext, pEKeyUsage))
			{
				_tprintf(TEXT("[i] Removed EKU\n"));
			}
			else
			{
				ErrorHandler("CertSetEnhancedKeyUsage Failed");
			}
			CryptMemFree(pEKeyUsage);
		}
		free(pszName);
	}

	// Clean Up
	if (pCertContext)
		CertFreeCertificateContext(pCertContext);
	if (hStoreHandle)
	{
		if (!CertCloseStore(hStoreHandle, 0))
			ErrorHandler("Failed CertCloseStorn");
	}
}


int _tmain(int argc, TCHAR* argv[])
{
	LPTSTR szSubjectName = NULL; // TEXT("COMODO RSA Certification Authority");
	LPTSTR pszStoreName = NULL; //  TEXT("Root");

	if (argc < 3)
	{
		_ftprintf(stderr, TEXT("USAGE: %s StoreName CertificateSubject\n"), argv[0]);
		_ftprintf(stderr, TEXT("For example:\n"));
		_ftprintf(stderr, TEXT("%s Root \"COMODO RSA Certification Authority\"\n"), argv[0]);
		return 1;
	}
	pszStoreName = argv[1];
	szSubjectName = argv[2];

	_tprintf(TEXT("Store: %s\n"), argv[1]);
	_tprintf(TEXT("Subject: %s\n"), argv[2]);
	RemoveAllEnhancedKeyUsages(pszStoreName, szSubjectName);

	// Return
    return 0;
}





