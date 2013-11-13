#include "trace_parser.h"
#include "trace_mapping.h"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

using namespace std;
void print_help();

int main(int argc, char* argv[]){
	try{
		po::options_description desc("Using the following options");
		desc.add_options()
			("help", "produce help msg")
			("pcap", "parse pcap src")
			("syn-p", "generate per packet based synthetic traces: need --head-dir, --ref-dir, optional --dst-dir, --scale")
			("syn-f", "generate per flow based synthetic traces: need --head-dir, --ref-dir, optional --dst-dir")
			("src-dir", po::value<string>(), "source directory for pcap files to get processed")
			("head-dir", po::value<string>(), "headers generated by classbench")
			("ref-dir", po::value<string>(), "reference directory")
			("dst-dir", po::value<string>()->default_value("syn_traces.txt"))
			("scale", po::value<size_t>()->default_value(0), "specify how many entries to generate")
			("map-arr", "specify whether directly map the arrival time or generate Poisson")
			("arr-intv", po::value<double>()->default_value(0.001), "specify the inter-arrival time for Poisson generation")
			;
			
		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.count("help")){
			print_help();
			return 0;
		}

		if (vm.count("pcap")){
			if (vm.count("src-dir")){
				ParseTrace(vm["src-dir"].as<string>().c_str(), vm["dst-dir"].as<string>().c_str(), vm["scale"].as<size_t>());
				return 0;
			}
			else{
				cout<<"ERROR: missing src directory, see help for details"<<endl;
				return 1;
			}
		}
		else if (vm.count("syn-p")){
			if (vm.count("ref-dir") && vm.count("head-dir")){
				Pack_mapping(vm["dst-dir"].as<string>().c_str(), vm["head-dir"].as<string>().c_str(), vm["ref-dir"].as<string>().c_str(), vm["scale"].as<size_t>());
				return 0;
			}
			else{
				cout<<"ERORR: missing parameters --ref-dir or --head-dir"<<endl;
				return 1;
			}
		}
		else if (vm.count("syn-f")){
			if (vm.count("ref-dir") && vm.count("head-dir")){
				if (vm.count("map-arr")){
					Flow_mapping(vm["dst-dir"].as<string>().c_str(), vm["head-dir"].as<string>().c_str(), vm["ref-dir"].as<string>().c_str(), vm["scale"].as<size_t>());
				}
				else{
					Flow_mapping(vm["dst-dir"].as<string>().c_str(), vm["head-dir"].as<string>().c_str(), vm["ref-dir"].as<string>().c_str(), vm["scale"].as<size_t>(), true, vm["arr-intv"].as<double>());
				}
				return 0;
			}
			else{
				cout<<"ERORR: missing parameters --ref-dir or --head-dir"<<endl;
				return 1;
			}
		}
		else{
			cout<<"missing core option. See --help for details."<<endl;
		}
	
	}
	catch(exception& e){
		cerr<<"error: "<<e.what()<<"\n";
		return 1;
	}
	catch(...){
		cerr<<"Exception of unknown type!\n";
	}
	return 0;
}


void print_help(){
	cout<<"Using the following options:"<<std::endl;
	cout<<"-help"<<std::endl;
	cout<<"  Get introduction to use this pre_processor tool."<<endl;
	cout<<endl;
	cout<<"--pcap [--src-dir] [--scale] [--dst-dir] "<<endl;
	cout<<"  Function: Parse pcap file to readable format for ofswitch simulation."<<endl;
	cout<<"  The trace files need to be tcpdump format in order to be recognized."<<endl;
	cout<<"  Options: Use [--src-dir] to specify the directory of traces, and [dst_file] to specify the file to save."<<endl;
	cout<<"           Use [--scale] to specify the maximum No. of packets to process, default is infinity."<<endl;
	cout<<endl;
	
	cout<<"--syn_p [--head-dir] [--ref-dir] [--scale] [--dst-dir] "<<endl;
	cout<<"  Function: Use headers to generate synthetic traces. Timestamp and packet replication is mapped from real traces."<<endl;
	cout<<"  The reference real traces is generated by the 'python trace aggregator' in form of .tar.gz"<<endl;
	cout<<"  Options: Use [--dst-dir] to specify the file to save." <<endl;
	cout<<"           Use [--head-dir] to specify the synthetic header file generated by classbench." <<endl;
	cout<<"           Use [--ref-dir] to specify the directory of the real traces."<<endl;
	cout<<"           Use [--scale] to specify the maximum No. of packets generated, default is infinity."<<endl;
	cout<<endl;

	cout<<"--syn_f [--head-dir] [--ref-dir] [--scale] [--dst-dir] "<<endl;
	cout<<"  Function: Use headers to generate synthetic flows. Timestamp is generated in a Poisson process with mean measured from real traces. Duration is mapped from real traces."<<endl;
	cout<<"  Options: Use [--dst-dir] to specify the file to save." <<endl;
	cout<<"           Use [--head-dir] to specify the synthetic header file generated by classbench." <<endl;
	cout<<"           Use [--ref-dir] to specify the directory of the flow information dir."<<endl;
	cout<<"           Use [--scale] to specify the maximum No. of flows generated, default is infinity."<<endl;
	cout<<endl;	
}
