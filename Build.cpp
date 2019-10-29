/*
 * Implementation of "Improved Compressed String Dictionaries" (CIKM'19)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _BUILD_CPP
#define _BUILD_CPP

#include <fstream>
#include <iostream>
using namespace std;

#include "StringDictionary.h"
#include "iterators/IteratorDictStringPlain.h"
#include "utils/LogSequence.h"



void checkDict()
{
	cerr << endl;
	cerr << " ******************************************************************************** " << endl;
	cerr << " *** Checks the given file because it does not represent any valid dictionary *** " << endl;
	cerr << " ******************************************************************************** " << endl;
	cerr << endl;
}

void checkFile()
{
	cerr << endl;
	cerr << " ****************************************************************** " << endl;
	cerr << " *** Checks the given path because it does not contain any file *** " << endl;
	cerr << " ****************************************************************** " << endl;
	cerr << endl;
}

void useBuild()
{

	cerr << endl;
	cerr << " ************************************************************************** " << endl;
	cerr << " *** BUILD script for indexing string dictionaries in compressed space. *** " << endl;
	cerr << " ************************************************************************** " << endl;
	cerr << endl;
	cerr << " ----- ./Build <type> <parameters> <in> <out>" << endl;

	cerr << " type: 9 => Build MM PFC dictionary" << endl;
	cerr << " \t <in> : input file containing the set of '\\0'-delimited strings." << endl;
	cerr << " \t <out> : output file for storing the dictionary." << endl;
	cerr << endl;
}

				
int 
main(int argc, char* argv[])
{
	if (argc > 1)
	{
		int type = atoi(argv[1]);

		switch (type)
		{
			
			case 91:
			{
				if (argc != 4) { useBuild(); break; }

				ifstream in(argv[2]);
				if (in.good())
				{
					in.seekg(0,ios_base::end);
					size_t lenStr = in.tellg()/sizeof(uchar);
					in.seekg(0,ios_base::beg);

					uchar *str = loadValue<uchar>(in, lenStr);
					IteratorDictString *it = new IteratorDictStringPlain(str, lenStr-1);
					in.close();

					// MM-PFC compression
					StringDictionary *dict = new StringDictionaryMMPFCPLAIN(it);
					delete it;

					string filename = string(argv[3])+ string(".mmpfcplain");
					ofstream out((char*)filename.c_str());
					dict->save(out);
					out.close();
					delete dict;
				} else checkFile();
				break;
			}
			case 911:
			{
				if (argc != 4) { useBuild(); break; }

				ifstream in(argv[2]);
				if (in.good())
				{
					in.seekg(0,ios_base::end);
					size_t lenStr = in.tellg()/sizeof(uchar);
					in.seekg(0,ios_base::beg);

					uchar *str = loadValue<uchar>(in, lenStr);
					IteratorDictString *it = new IteratorDictStringPlain(str, lenStr-1);
					in.close();

					// MM-PFC compression
					StringDictionary *dict = new StringDictionaryMMPFCPLAINLlcp(it);
					delete it;

					string filename = string(argv[3])+ string(".mmpfcplainllcp");
					ofstream out((char*)filename.c_str());
					dict->save(out);
					out.close();
					delete dict;
				} else checkFile();
				break;
			}
			case 912:
			{
				if (argc != 4) { useBuild(); break; }

				ifstream in(argv[2]);
				if (in.good())
				{
					in.seekg(0,ios_base::end);
					size_t lenStr = in.tellg()/sizeof(uchar);
					in.seekg(0,ios_base::beg);

					uchar *str = loadValue<uchar>(in, lenStr);
					IteratorDictString *it = new IteratorDictStringPlain(str, lenStr-1);
					in.close();

					// MM-PFC compression
					StringDictionary *dict = new StringDictionaryMMPFCPLAINRlcp(it);
					delete it;

					string filename = string(argv[3])+ string(".mmpfcplainrlcp");
					ofstream out((char*)filename.c_str());
					dict->save(out);
					out.close();
					delete dict;
				} else checkFile();
				break;
			}
			case 92:
			{
				if (argc != 4) { useBuild(); break; }

				ifstream in(argv[2]);
				if (in.good())
				{
					in.seekg(0,ios_base::end);
					size_t lenStr = in.tellg()/sizeof(uchar);
					in.seekg(0,ios_base::beg);

					uchar *str = loadValue<uchar>(in, lenStr);
					IteratorDictString *it = new IteratorDictStringPlain(str, lenStr-1);
					in.close();

					// MM-PFC compression
					StringDictionary *dict = new StringDictionaryMMPFCbSD(it);
					delete it;

					string filename = string(argv[3])+ string(".mmpfcbsd");
					ofstream out((char*)filename.c_str());
					dict->save(out);
					out.close();
					delete dict;
				} else checkFile();
				break;
			}
			case 921:
			{
				if (argc != 4) { useBuild(); break; }

				ifstream in(argv[2]);
				if (in.good())
				{
					in.seekg(0,ios_base::end);
					size_t lenStr = in.tellg()/sizeof(uchar);
					in.seekg(0,ios_base::beg);

					uchar *str = loadValue<uchar>(in, lenStr);
					IteratorDictString *it = new IteratorDictStringPlain(str, lenStr-1);
					in.close();

					// MM-PFC compression
					StringDictionary *dict = new StringDictionaryMMPFCbSDLlcp(it);
					delete it;

					string filename = string(argv[3])+ string(".mmpfcbsdllcp");
					ofstream out((char*)filename.c_str());
					dict->save(out);
					out.close();
					delete dict;
				} else checkFile();
				break;
			}
			case 922:
			{
				if (argc != 4) { useBuild(); break; }

				ifstream in(argv[2]);
				if (in.good())
				{
					in.seekg(0,ios_base::end);
					size_t lenStr = in.tellg()/sizeof(uchar);
					in.seekg(0,ios_base::beg);

					uchar *str = loadValue<uchar>(in, lenStr);
					IteratorDictString *it = new IteratorDictStringPlain(str, lenStr-1);
					in.close();

					// MM-PFC compression
					StringDictionary *dict = new StringDictionaryMMPFCbSDRlcp(it);
					delete it;

					string filename = string(argv[3])+ string(".mmpfcbsdrlcp");
					ofstream out((char*)filename.c_str());
					dict->save(out);
					out.close();
					delete dict;
				} else checkFile();
				break;
			}

			case 93:
			{
				if (argc != 4) { useBuild(); break; }

				ifstream in(argv[2]);
				if (in.good())
				{
					in.seekg(0,ios_base::end);
					size_t lenStr = in.tellg()/sizeof(uchar);
					in.seekg(0,ios_base::beg);

					uchar *str = loadValue<uchar>(in, lenStr);
					IteratorDictString *it = new IteratorDictStringPlain(str, lenStr-1);
					in.close();

					// MM-PFC compression
					StringDictionary *dict = new StringDictionaryMMRPFCbSD(it);
					delete it;

					string filename = string(argv[3])+ string(".mmrpfcbsd");
					ofstream out((char*)filename.c_str());
					dict->save(out);
					out.close();
					delete dict;
				} else checkFile();
				break;
			}
			case 931:
			{
				if (argc != 4) { useBuild(); break; }

				ifstream in(argv[2]);
				if (in.good())
				{
					in.seekg(0,ios_base::end);
					size_t lenStr = in.tellg()/sizeof(uchar);
					in.seekg(0,ios_base::beg);

					uchar *str = loadValue<uchar>(in, lenStr);
					IteratorDictString *it = new IteratorDictStringPlain(str, lenStr-1);
					in.close();

					// MM-PFC compression
					StringDictionary *dict = new StringDictionaryMMRPFCbSDLlcp(it);
					delete it;

					string filename = string(argv[3])+ string(".mmrpfcbsdllcp");
					ofstream out((char*)filename.c_str());
					dict->save(out);
					out.close();
					delete dict;
				} else checkFile();
				break;
			}
			case 932:
			{
				if (argc != 4) { useBuild(); break; }

				ifstream in(argv[2]);
				if (in.good())
				{
					in.seekg(0,ios_base::end);
					size_t lenStr = in.tellg()/sizeof(uchar);
					in.seekg(0,ios_base::beg);

					uchar *str = loadValue<uchar>(in, lenStr);
					IteratorDictString *it = new IteratorDictStringPlain(str, lenStr-1);
					in.close();

					// MM-PFC compression
					StringDictionary *dict = new StringDictionaryMMRPFCbSDRlcp(it);
					delete it;

					string filename = string(argv[3])+ string(".mmrpfcbsdrlcp");
					ofstream out((char*)filename.c_str());
					dict->save(out);
					out.close();
					delete dict;
				} else checkFile();
				break;
			}

			case 95:
				{
					if (argc != 4) { useBuild(); break; }

					ifstream in(argv[2]);
					if (in.good())
					{
						in.seekg(0,ios_base::end);
						size_t lenStr = in.tellg()/sizeof(uchar);
						in.seekg(0,ios_base::beg);

						uchar *str = loadValue<uchar>(in, lenStr);
						IteratorDictString *it = new IteratorDictStringPlain(str, lenStr-1);
						in.close();

						// MM-PFC compression
						StringDictionary *dict = new StringDictionaryMMRPFCbSDDAC(it);
						delete it;

						string filename = string(argv[3])+ string(".mmrpfcbsddac");
						ofstream out((char*)filename.c_str());
						dict->save(out);
						out.close();
						delete dict;
					} else checkFile();
					break;
				}
			case 951:
				{
					if (argc != 4) { useBuild(); break; }

					ifstream in(argv[2]);
					if (in.good())
					{
						in.seekg(0,ios_base::end);
						size_t lenStr = in.tellg()/sizeof(uchar);
						in.seekg(0,ios_base::beg);

						uchar *str = loadValue<uchar>(in, lenStr);
						IteratorDictString *it = new IteratorDictStringPlain(str, lenStr-1);
						in.close();

						// MM-PFC compression
						StringDictionary *dict = new StringDictionaryMMRPFCbSDDACLlcp(it);
						delete it;

						string filename = string(argv[3])+ string(".mmrpfcbsddacllcp");
						ofstream out((char*)filename.c_str());
						dict->save(out);
						out.close();
						delete dict;
					} else checkFile();
					break;
				}
			case 952:
				{
					if (argc != 4) { useBuild(); break; }

					ifstream in(argv[2]);
					if (in.good())
					{
						in.seekg(0,ios_base::end);
						size_t lenStr = in.tellg()/sizeof(uchar);
						in.seekg(0,ios_base::beg);

						uchar *str = loadValue<uchar>(in, lenStr);
						IteratorDictString *it = new IteratorDictStringPlain(str, lenStr-1);
						in.close();

						// MM-PFC compression
						StringDictionary *dict = new StringDictionaryMMRPFCbSDDACRlcp(it);
						delete it;

						string filename = string(argv[3])+ string(".mmrpfcbsddacrlcp");
						ofstream out((char*)filename.c_str());
						dict->save(out);
						out.close();
						delete dict;
					} else checkFile();
					break;
				}

			case 96:
				{
					if (argc != 4) { useBuild(); break; }

					ifstream in(argv[2]);
					if (in.good())
					{
						in.seekg(0,ios_base::end);
						size_t lenStr = in.tellg()/sizeof(uchar);
						in.seekg(0,ios_base::beg);

						uchar *str = loadValue<uchar>(in, lenStr);
						IteratorDictString *it = new IteratorDictStringPlain(str, lenStr-1);
						in.close();

						// MM-PFC compression
						StringDictionary *dict = new StringDictionaryMMRPFCbSDDAC2(it);
						delete it;

						string filename = string(argv[3])+ string(".mmrpfcbsddac2");
						ofstream out((char*)filename.c_str());
						dict->save(out);
						out.close();
						delete dict;
					} else checkFile();
					break;
				}
			case 961:
				{
					if (argc != 4) { useBuild(); break; }

					ifstream in(argv[2]);
					if (in.good())
					{
						in.seekg(0,ios_base::end);
						size_t lenStr = in.tellg()/sizeof(uchar);
						in.seekg(0,ios_base::beg);

						uchar *str = loadValue<uchar>(in, lenStr);
						IteratorDictString *it = new IteratorDictStringPlain(str, lenStr-1);
						in.close();

						// MM-PFC compression
						StringDictionary *dict = new StringDictionaryMMRPFCbSDDAC2Llcp(it);
						delete it;

						string filename = string(argv[3])+ string(".mmrpfcbsddac2llcp");
						ofstream out((char*)filename.c_str());
						dict->save(out);
						out.close();
						delete dict;
					} else checkFile();
					break;
				}
			case 962:
				{
					if (argc != 4) { useBuild(); break; }

					ifstream in(argv[2]);
					if (in.good())
					{
						in.seekg(0,ios_base::end);
						size_t lenStr = in.tellg()/sizeof(uchar);
						in.seekg(0,ios_base::beg);

						uchar *str = loadValue<uchar>(in, lenStr);
						IteratorDictString *it = new IteratorDictStringPlain(str, lenStr-1);
						in.close();

						// MM-PFC compression
						StringDictionary *dict = new StringDictionaryMMRPFCbSDDAC2Rlcp(it);
						delete it;

						string filename = string(argv[3])+ string(".mmrpfcbsddac2rlcp");
						ofstream out((char*)filename.c_str());
						dict->save(out);
						out.close();
						delete dict;
					} else checkFile();
					break;
				}

			case 97:
			{
				if (argc != 4) { useBuild(); break; }

				ifstream in(argv[2]);
				if (in.good())
				{
					in.seekg(0,ios_base::end);
					size_t lenStr = in.tellg()/sizeof(uchar);
					in.seekg(0,ios_base::beg);

					uchar *str = loadValue<uchar>(in, lenStr);
					IteratorDictString *it = new IteratorDictStringPlain(str, lenStr-1);
					in.close();

					// MM-PFC compression
					StringDictionary *dict = new StringDictionaryMMRPFCDACVLS(it);
					delete it;

					string filename = string(argv[3])+ string(".mmrpfcdacvls");
					ofstream out((char*)filename.c_str());
					dict->save(out);
					out.close();
					delete dict;
				} else checkFile();
				break;
			}
			case 971:
			{
				if (argc != 4) { useBuild(); break; }

				ifstream in(argv[2]);
				if (in.good())
				{
					in.seekg(0,ios_base::end);
					size_t lenStr = in.tellg()/sizeof(uchar);
					in.seekg(0,ios_base::beg);

					uchar *str = loadValue<uchar>(in, lenStr);
					IteratorDictString *it = new IteratorDictStringPlain(str, lenStr-1);
					in.close();

					// MM-PFC compression
					StringDictionary *dict = new StringDictionaryMMRPFCDACVLSLlcp(it);
					delete it;

					string filename = string(argv[3])+ string(".mmrpfcdacvlsllcp");
					ofstream out((char*)filename.c_str());
					dict->save(out);
					out.close();
					delete dict;
				} else checkFile();
				break;
			}
			case 972:
			{
				if (argc != 4) { useBuild(); break; }

				ifstream in(argv[2]);
				if (in.good())
				{
					in.seekg(0,ios_base::end);
					size_t lenStr = in.tellg()/sizeof(uchar);
					in.seekg(0,ios_base::beg);

					uchar *str = loadValue<uchar>(in, lenStr);
					IteratorDictString *it = new IteratorDictStringPlain(str, lenStr-1);
					in.close();

					// MM-PFC compression
					StringDictionary *dict = new StringDictionaryMMRPFCDACVLSRlcp(it);
					delete it;

					string filename = string(argv[3])+ string(".mmrpfcdacvlsrlcp");
					ofstream out((char*)filename.c_str());
					dict->save(out);
					out.close();
					delete dict;
				} else checkFile();
				break;
			}

			case 98:
			{
				if (argc != 4) { useBuild(); break; }

				ifstream in(argv[2]);
				if (in.good())
				{
					in.seekg(0,ios_base::end);
					size_t lenStr = in.tellg()/sizeof(uchar);
					in.seekg(0,ios_base::beg);

					uchar *str = loadValue<uchar>(in, lenStr);
					IteratorDictString *it = new IteratorDictStringPlain(str, lenStr-1);
					in.close();

					// MM-PFC compression
					StringDictionary *dict = new StringDictionaryMMRPFCDAC2VLS(it);
					delete it;

					string filename = string(argv[3])+ string(".mmrpfcdac2vls");
					ofstream out((char*)filename.c_str());
					dict->save(out);
					out.close();
					delete dict;
				} else checkFile();
				break;
			}
			case 981:
			{
				if (argc != 4) { useBuild(); break; }

				ifstream in(argv[2]);
				if (in.good())
				{
					in.seekg(0,ios_base::end);
					size_t lenStr = in.tellg()/sizeof(uchar);
					in.seekg(0,ios_base::beg);

					uchar *str = loadValue<uchar>(in, lenStr);
					IteratorDictString *it = new IteratorDictStringPlain(str, lenStr-1);
					in.close();

					// MM-PFC compression
					StringDictionary *dict = new StringDictionaryMMRPFCDAC2VLSLlcp(it);
					delete it;

					string filename = string(argv[3])+ string(".mmrpfcdac2vlsllcp");
					ofstream out((char*)filename.c_str());
					dict->save(out);
					out.close();
					delete dict;
				} else checkFile();
				break;
			}
			case 982:
			{
				if (argc != 4) { useBuild(); break; }

				ifstream in(argv[2]);
				if (in.good())
				{
					in.seekg(0,ios_base::end);
					size_t lenStr = in.tellg()/sizeof(uchar);
					in.seekg(0,ios_base::beg);

					uchar *str = loadValue<uchar>(in, lenStr);
					IteratorDictString *it = new IteratorDictStringPlain(str, lenStr-1);
					in.close();

					// MM-PFC compression
					StringDictionary *dict = new StringDictionaryMMRPFCDAC2VLSRlcp(it);
					delete it;

					string filename = string(argv[3])+ string(".mmrpfcdac2vlsrlcp");
					ofstream out((char*)filename.c_str());
					dict->save(out);
					out.close();
					delete dict;
				} else checkFile();
				break;
			}


			default:
			{
				useBuild();
				break;
			}
		}
	}
	else
	{
		useBuild();
	}
}

#endif  /* _BUILD_CPP */

