
#include "CZip.h"

extern "C"
{
#include "zlib.h"
#include "zip.h"
#include "unzip.h"
}

//-------------------------------------------------------------------------------------------------

CZip::CZip(const QString& sFileName)
: m_sFileName(sFileName)
{
}

//-------------------------------------------------------------------------------------------------

CZip::~CZip()
{
}

//-------------------------------------------------------------------------------------------------

int CZip::zipFiles(QStringList sFiles)
{
	char szZipFileName [256];
	strcpy(szZipFileName, m_sFileName.toStdString().c_str());

	zipFile pFile = zipOpen(szZipFileName, APPEND_STATUS_CREATE);

	if (pFile != NULL)
	{
		foreach (QString sFile, sFiles)
		{
			zip_fileinfo info;

			info.tmz_date.tm_year = 2000;
			info.tmz_date.tm_mon = 1;
			info.tmz_date.tm_mday = 1;
			info.tmz_date.tm_hour = 0;
			info.tmz_date.tm_min = 0;
			info.tmz_date.tm_sec = 0;
			info.dosDate = 0;
			info.internal_fa = 0;
			info.external_fa = 0;

			char szFullQualifiedFileName [256];
			strcpy(szFullQualifiedFileName, sFile.toLatin1().constData());

			char szFileName [256];
			QFileInfo tFileInfo(sFile);
			strcpy(szFileName, tFileInfo.fileName().toLatin1().constData());

			if (zipOpenNewFileInZip(pFile, szFileName, &info, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION) == 0)
			{
				FILE* f = fopen(szFullQualifiedFileName, "rb");

				if (f != NULL)
				{
					fseek(f, 0, SEEK_END);
					int size = ftell(f);
					fseek(f, 0, SEEK_SET);

					char* buffer = new char[size];
					fread(buffer, 1, size, f);

					zipWriteInFileInZip(pFile, buffer, size);

					delete [] buffer;
					fclose(f);
				}

				zipCloseFileInZip(pFile);
			}
		}

		zipClose(pFile, NULL);
	}
	else
	{
		return 1;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------

QStringList CZip::getZipFileList()
{
	QStringList slFiles;
	char szZipFileName [256];
	strcpy(szZipFileName, m_sFileName.toLatin1().constData());

	unzFile pFile = unzOpen(szZipFileName);

	if (pFile != NULL)
	{
		unz_file_info tCurrentFileInfo;
		char szCurrentFileName [256];

		if (unzGoToFirstFile(pFile) == UNZ_OK)
		{
			if (unzGetCurrentFileInfo(
				pFile,
				&tCurrentFileInfo,
				szCurrentFileName, sizeof(szCurrentFileName),
				NULL, 0,
				NULL, 0
				) == UNZ_OK)
			{
				slFiles.append(QString(szCurrentFileName));
			}

			while (unzGoToNextFile(pFile) == UNZ_OK)
			{
				if (unzGetCurrentFileInfo(
					pFile,
					&tCurrentFileInfo,
					szCurrentFileName, sizeof(szCurrentFileName),
					NULL, 0,
					NULL, 0
					) == UNZ_OK)
				{
					slFiles.append(QString(szCurrentFileName));
				}
			}
		}

		unzClose(pFile);
	}

	return slFiles;
}

//-------------------------------------------------------------------------------------------------

QByteArray CZip::getZipFileContent(QString& sFile)
{
	QByteArray baReturnValue;
	char szZipFileName [256];
	strcpy(szZipFileName, m_sFileName.toLatin1().constData());

	unzFile pFile = unzOpen(szZipFileName);

	if (pFile != NULL)
	{
		if (unzLocateFile(pFile, sFile.toLatin1().constData(), 2) == UNZ_OK)
		{
			unz_file_info tCurrentFileInfo;
			char szCurrentFileName [256];

			if (unzGetCurrentFileInfo(
				pFile,
				&tCurrentFileInfo,
				szCurrentFileName, sizeof(szCurrentFileName),
				NULL, 0,
				NULL, 0
				) == UNZ_OK)
			{
				qint64 uiSize = tCurrentFileInfo.uncompressed_size;

				baReturnValue.resize(uiSize);

				if (unzOpenCurrentFile(pFile) == UNZ_OK)
				{
					if (unzReadCurrentFile(pFile, baReturnValue.data(), uiSize) == 0)
					{
					}

					unzCloseCurrentFile(pFile);
				}
			}
		}

		unzClose(pFile);
	}

	return baReturnValue;
}

//-------------------------------------------------------------------------------------------------

#define GZIP_WINDOWS_BIT	(MAX_WBITS + 16)
#define GZIP_CHUNK_SIZE		(32 * 1024)

/**
 * @brief Compresses the given buffer using the standard GZIP algorithm
 * @param input The buffer to be compressed
 * @param output The result of the compression
 * @param level The compression level to be used (@c 0 = no compression, @c 9 = max, @c -1 = default)
 * @return @c true if the compression was successful, @c false otherwise
 */
bool gzipCompress(QByteArray input, QByteArray &output, int level)
{
    // Prepare output
    output.clear();

    // Is there something to do?
    if(input.length())
    {
        // Declare vars
        int flush = 0;

        // Prepare deflater status
        z_stream strm;
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;
        strm.avail_in = 0;
        strm.next_in = Z_NULL;

        // Initialize deflater
        int ret = deflateInit2(&strm, qMax(-1, qMin(9, level)), Z_DEFLATED, GZIP_WINDOWS_BIT, 8, Z_DEFAULT_STRATEGY);

        if (ret != Z_OK)
            return(false);

        // Prepare output
        output.clear();

        // Extract pointer to input data
        char *input_data = input.data();
        int input_data_left = input.length();

        // Compress data until available
        do {
            // Determine current chunk size
            int chunk_size = qMin(GZIP_CHUNK_SIZE, input_data_left);

            // Set deflater references
            strm.next_in = (unsigned char*)input_data;
            strm.avail_in = chunk_size;

            // Update interval variables
            input_data += chunk_size;
            input_data_left -= chunk_size;

            // Determine if it is the last chunk
            flush = (input_data_left <= 0 ? Z_FINISH : Z_NO_FLUSH);

            // Deflate chunk and cumulate output
            do {

                // Declare vars
                char out[GZIP_CHUNK_SIZE];

                // Set deflater references
                strm.next_out = (unsigned char*)out;
                strm.avail_out = GZIP_CHUNK_SIZE;

                // Try to deflate chunk
                ret = deflate(&strm, flush);

                // Check errors
                if(ret == Z_STREAM_ERROR)
                {
                    // Clean-up
                    deflateEnd(&strm);

                    // Return
                    return(false);
                }

                // Determine compressed size
                int have = (GZIP_CHUNK_SIZE - strm.avail_out);

                // Cumulate result
                if(have > 0)
                    output.append((char*)out, have);

            } while (strm.avail_out == 0);

        } while (flush != Z_FINISH);

        // Clean-up
        (void)deflateEnd(&strm);

        // Return
        return(ret == Z_STREAM_END);
    }
    else
        return(true);
}

//-------------------------------------------------------------------------------------------------

/**
 * @brief Decompresses the given buffer using the standard GZIP algorithm
 * @param input The buffer to be decompressed
 * @param output The result of the decompression
 * @return @c true if the decompression was successfull, @c false otherwise
 */
bool gzipDecompress(QByteArray input, QByteArray &output)
{
    // Prepare output
    output.clear();

    // Is there something to do?
    if(input.length() > 0)
    {
        // Prepare inflater status
        z_stream strm;
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;
        strm.avail_in = 0;
        strm.next_in = Z_NULL;

        // Initialize inflater
        int ret = inflateInit2(&strm, GZIP_WINDOWS_BIT);

        if (ret != Z_OK)
            return(false);

        // Extract pointer to input data
        char *input_data = input.data();
        int input_data_left = input.length();

        // Decompress data until available
        do {
            // Determine current chunk size
            int chunk_size = qMin(GZIP_CHUNK_SIZE, input_data_left);

            // Check for termination
            if(chunk_size <= 0)
                break;

            // Set inflater references
            strm.next_in = (unsigned char*)input_data;
            strm.avail_in = chunk_size;

            // Update interval variables
            input_data += chunk_size;
            input_data_left -= chunk_size;

            // Inflate chunk and cumulate output
            do {

                // Declare vars
                char out[GZIP_CHUNK_SIZE];

                // Set inflater references
                strm.next_out = (unsigned char*)out;
                strm.avail_out = GZIP_CHUNK_SIZE;

                // Try to inflate chunk
                ret = inflate(&strm, Z_NO_FLUSH);

                switch (ret) {
                case Z_NEED_DICT:
                    ret = Z_DATA_ERROR;
                case Z_DATA_ERROR:
                case Z_MEM_ERROR:
                case Z_STREAM_ERROR:
                    // Clean-up
                    inflateEnd(&strm);

                    // Return
                    return(false);
                }

                // Determine decompressed size
                int have = (GZIP_CHUNK_SIZE - strm.avail_out);

                // Cumulate result
                if(have > 0)
                    output.append((char*)out, have);

            } while (strm.avail_out == 0);

        } while (ret != Z_STREAM_END);

        // Clean-up
        inflateEnd(&strm);

        // Return
        return (ret == Z_STREAM_END);
    }
    else
        return(true);
}
