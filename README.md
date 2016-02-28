# CertiDisableAllPurposes
A command line windows utility to disable *all* enhanced key usage purposes for the specified certificate. This was written because it seemed easier to interact with certificate stored using WINAPI than PowerShell.

## Usage
    CertDisableAllPurposes.exe StoreName CertificateSubject

For example:

    CertDisableAllPurposes.exe Root "COMODO RSA Certification Authority"

## Output
When run, a log of activity is printed.

    C:\Temp>CertDisableAllPurposes.exe Root "COMODO RSA Certification Authority"
    Store: Root
    Subject: COMODO RSA Certification Authority
    [i] Successfully opened certificate store 'Root'
    [i] Located certificate COMODO RSA Certification Authority
    [i] Removed EKU
