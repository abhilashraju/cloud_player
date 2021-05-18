#include "scloud_xml_parser.h"

inline SCloudParser& get_native_parser(){
    static SCloudParser nativeparser(std::stringstream(R"xml(
        <cloudUiDescription defaultScreen="busy_screen" name="smarttasks_native">
        <screen id="busy_screen" allowCache="false">
            <text labelText= "{$status}" order="3" />
            <preFetchImage src= ":/images/progress_spinner.json"/>
        </screen> 
        <screen id="error_screen" allowCache="false">
            <static location="heading" labelText="None"/>
            <static location="info" labelText="{$error}" />
            
        </screen>  
        </cloudUiDescription>
    )xml"));
    return nativeparser;

}