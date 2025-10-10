#pragma once


namespace IronMan::Core
{
	

	class Package
	{
	private:
		/*
		 *	STRUCT
		 */
		ALIGNMENT struct PackageHeader //The header for the .pak file, the only thing not encyrpted
		{
			char fileID[6]; //Identifier to make sure this is the type of .pak we want (and not some other sort of compression someone else made), I use "DBPAK"
			char version; //Version of the format
			int numberFiles; //Total number of files
			bool additionEncrypt; //Random, whether to add or subtract "encryptVal" from each byte
			char encryptVal; //Random, value that is added or subtracted from each byte
		};

		ALIGNMENT struct FileEntry //Basically an entry in a table of contents for each file stored in the .pak file
		{
			char name[50]; //name of the file, must be unique from other files
			char fullname[150]; //name of the file + the folder it is in
			unsigned int size;		//size of the file in bytes
			unsigned int offset;	//offset of where the files in located in the .pak file in bytes
		};

		struct FileEntryHash
		{
			hash_t name;
			hash_t fullname;
			unsigned int size;
			unsigned int offset;
		};


	public:
		/*
		 *		STATIC INTERFACE
		 */
		static Package*		GetPackage();

		~Package();

		bool Load(std::string packagePath);
		FileEntryHash* FindFileEntryHash(hash_t fileNameHash);
		size_t FindFile(hash_t fileNameHash, void** outData);
		void Release();
	private:
		Package();


	private:
		/*
		 *		STATIC
		 */
		static Package*		m_Instance;

		ptr_t fileBuf = 0;
		ptr_t fileSize = 0;
		std::istringstream m_PackageFile;
		PackageHeader m_Header;				//the header
		std::vector<FileEntryHash> m_Entries;	//table of contents of all the entries
		bool m_bLoaded;
	};
}