<?php 

	//IDs of lab which has both esim and freeeda version
	//$convertedlabArray=array(2,30);


	function lab_migration_run_form($form, &$form_state) {
			$convertedlabArray=array(2,30);
		$options_first =_list_of_labs();
		$options_two = _ajax_get_experiment_list();
		
		$select_two = isset($form_state['values']['lab_experiment_list']) ? $form_state['values']['lab_experiment_list'] : 
		key($options_two);
 		$url_lab_id = (int)arg(1);
			if(!$url_lab_id){
				$selected = isset($form_state['values']['lab']) ? $form_state['values']['lab'] : key($options_first);
			}elseif($url_lab_id == ''){
				$selected = 0;
			}else{
				$selected  = $url_lab_id;;
			}
    
		$form = array();
		$form['lab'] = array(
			'#type' => 'select',
			'#title' => t('Title of the lab'),
			'#options' =>  _list_of_labs(),
			'#default_value' => $selected,
			'#ajax' => array(
				'callback' => 'ajax_experiment_list_callback',
				),
		);
		if(!$url_lab_id){
		$form['selected_lab'] = array(
			'#type' => 'item',
			'#markup' => '<div id="ajax_selected_lab"></div>',
		);
		$form['selected_lab_freeeda'] = array(
			'#type' => 'item',
			'#markup' => '<div id="ajax_selected_lab_freeeda"></div>',
		);
		$form['selected_lab_pdf'] = array(
			'#type' => 'item',
			'#markup' => '<div id="ajax_selected_lab_pdf"></div>',
		);
		$form['lab_details'] = array(
			'#type' => 'item',
			'#markup' => '<div id="ajax_lab_details"></div>',
 		);
		$form['lab_experiment_list'] = array(
			'#type' => 'select',
			'#title' => t('Titile of the experiment'),
			'#options' =>  _ajax_get_experiment_list($selected),
			//'#default_value' => isset($form_state['values']['lab_experiment_list']) ? $form_state['values']['lab_experiment_list'] : '',
			'#ajax' => array(
				'callback' => 'ajax_solution_list_callback',
				),
			'#prefix' => '<div id="ajax_selected_experiment">',
			'#suffix' => '</div>',
			'#states' => array('invisible' => array(':input[name="lab"]' => array('value' => 0),),),  
		);
		$form['download_experiment'] = array(
			'#type' => 'item',
			'#markup' => '<div id="ajax_download_experiments"></div>',
		);
		$form['lab_solution_list'] = array(
			'#type' => 'select',
			'#title' => t('Solution'),
			'#options' =>   _ajax_get_solution_list($select_two),
			//'#default_value' => isset($form_state['values']['lab_solution_list']) ? 
			//$form_state['values']['lab_solution_list'] : '',
			'#ajax' => array(
				'callback' => 'ajax_solution_files_callback',
				),
			'#prefix' => '<div id="ajax_selected_solution">',
			'#suffix' => '</div>',
			'#states' => array('invisible' => array(':input[name="lab"]' => array('value' => 0),),),  
			);
		$form['download_solution'] = array(
			'#type' => 'item',
			'#markup' => '<div id="ajax_download_experiment_solution"></div>',
		);
		$form['edit_solution'] = array(
			'#type' => 'item',
			'#markup' => '<div id="ajax_edit_experiment_solution"></div>',
		);
		$form['solution_files'] = array(
			'#type' => 'item',
		//  '#title' => t('List of solution_files'),
			'#markup' => '<div id="ajax_solution_files"></div>',
			'#states' => array('invisible' => array(':input[name="lab"]' => array('value' => 0),),),  
			);
		}else{
			$lab_default_value = $url_lab_id;
		$form['selected_lab'] = array(
        '#type' => 'item',
        '#markup' => '<div id="ajax_selected_lab">'.l('Download Lab Solutions', 'lab_migration/download/lab/' . $lab_default_value).'</div>',
      
      );
   /*   $form['selected_lab_pdf'] = array(
        '#type' => 'item',
        '#markup' => '<div id="ajax_selected_lab_pdf">'. l('Download PDF of Lab Solutions', 'lab_migration/generate_lab/' . $lab_default_value . '/1') .'</div>',
      
      );*/

if(in_array($lab_default_value, $convertedlabArray)){
 $lab_details = _lab_information($lab_default_value);
 $book_name=str_replace(' ', '_', $lab_details->lab_title)."_FreeEDA_Version.zip";

$form['selected_lab_freeeda'] = array(
        '#type' => 'item',
        '#markup' => '<div id="ajax_selected_lab_freeeda">'.l('Download Lab Solutions (FreeEDA Version)', 'lab_migration_uploads/'.$book_name).'</div>',
      
      );
}else{
$form['selected_lab_freeeda'] = array(
        '#type' => 'item',
        '#markup' => '',
);
}
      $form['lab_details'] = array(
        '#type' => 'item',
        '#markup' => '<div id="ajax_lab_details">'._lab_details($lab_default_value).'</div>',
       );
     $form['lab_experiment_list'] = array(
        '#type' => 'select',
         '#title' => t('Titile of the experiment'),
        '#options' =>  _ajax_get_experiment_list($selected),
       // '#default_value' => isset($form_state['values']['lab_experiment_list']) ? $form_state['values']['lab_experiment_list'] : '',
        '#ajax' => array(
         'callback' => 'ajax_solution_list_callback',
        ),
        '#prefix' => '<div id="ajax_selected_experiment">',
        '#suffix' => '</div>',
        '#states' => array('invisible' => array(':input[name="lab"]' => array('value' => 0),),),  
      );
      $form['download_experiment'] = array(
        '#type' => 'item',
        '#markup' => '<div id="ajax_download_experiments"></div>',
      );
      $form['lab_solution_list'] = array(
        '#type' => 'select',
        '#title' => t('Solution'),
        '#options' =>   _ajax_get_solution_list($select_two),
        '#default_value' => isset($form_state['values']['lab_solution_list']) ? $form_state['values']['lab_solution_list'] : '',
        '#ajax' => array(
         'callback' => 'ajax_solution_files_callback',
        ),
        '#prefix' => '<div id="ajax_selected_solution">',
        '#suffix' => '</div>',
        '#states' => array('invisible' => array(':input[name="lab_experiment_list"]' => array('value' => 0),),),  
      );
		
		$form['download_solution'] = array(
        '#type' => 'item',
        '#markup' => '<div id="ajax_download_experiment_solution"></div>',
      );
      $form['edit_solution'] = array(
        '#type' => 'item',
        '#markup' => '<div id="ajax_edit_experiment_solution"></div>',
      );
      $form['solution_files'] = array(
        '#type' => 'item',
      //  '#title' => t('List of solution_files'),
        '#markup' => '<div id="ajax_solution_files"></div>',
        '#states' => array('invisible' => array(':input[name="lab_experiment_list"]' => array('value' => 0),),),  
      );
	}
    /*
        $form['message'] = array(
      '#type' => 'textarea',
      '#title' => t('If Dis-Approved please specify reason for Dis-Approval'),
      '#prefix' => '<div id= "message_submit">',   
      '#states' => array('invisible' => array(':input[name="lab"]' => array('value' => 0,),),), 
        
      );
  
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Submit'),      
      '#suffix' => '</div>',
      '#states' => array('invisible' => array(':input[name="lab"]' => array('value' => 0,),),),
      
    );*/
      return $form;
	}
		
function ajax_experiment_list_callback($form, $form_state) {
		$convertedlabArray=array(2,30);
      $commands = array();
      $lab_default_value = $form_state['values']['lab'];
      if($lab_default_value !=0){  
      $form['lab_details']['#markup'] = _lab_details($lab_default_value);
        $lab_details = _lab_information($lab_default_value);
        	$user_solution_provider = user_load($lab_details->solution_provider_uid);
        	//var_dump($lab_details->solution_provider_uid);
    	if ($lab_details->solution_provider_uid > 0) { 
    	 $commands[] = ajax_command_html('#ajax_selected_lab', l('Download Lab Solutions', 'lab_migration/download/lab/' . $lab_default_value));


    if(in_array($lab_default_value, $convertedlabArray)){
$lab_details = _lab_information($lab_default_value);
 $book_name=str_replace(' ', '_', $lab_details->lab_title)."_FreeEDA_Version.zip";
 $commands[] = ajax_command_html('#ajax_selected_lab_freeeda', l('Download Lab Solutions (FreeEDA Version)', 'lab_migration_uploads/'.$book_name));}
else{
 $commands[] = ajax_command_html('#ajax_selected_lab_freeeda', ''); 
} 
      //$commands[] = ajax_command_html('#ajax_selected_lab_pdf', l('Download PDF of Lab Solutions', 'lab_migration/generate_lab/' . $lab_default_value . '/1'));         
        
        }else{
         $commands[] = ajax_command_html('#ajax_selected_lab', '');      
         $commands[] = ajax_command_html('#ajax_selected_lab_pdf', '');  
         $commands[] = ajax_command_html('#ajax_selected_lab_freeeda', ''); 
        }
        
      $commands[] = ajax_command_html('#ajax_lab_details', _lab_details($lab_default_value));
      $form['lab_experiment_list']['#options'] = _ajax_get_experiment_list($lab_default_value);
      $commands[] = ajax_command_replace('#ajax_selected_experiment',drupal_render($form['lab_experiment_list']));
      $commands[] = ajax_command_html('#ajax_selected_solution', '');
      $commands[] = ajax_command_html('#ajax_solution_files', '');
      $commands[] = ajax_command_html('#ajax_download_experiment_solution', '');
      $commands[] = ajax_command_html('#ajax_edit_experiment_solution', '');
      $commands[] = ajax_command_html('#ajax_download_experiments', '');
     
      }else{
		   $form['lab_experiment_list']['#options'] = _ajax_get_experiment_list();
		   $commands[] = ajax_command_replace('#ajax_selected_experiment',drupal_render($form['lab_experiment_list']));

		   $commands[] = ajax_command_html('#ajax_lab_details', '');
		   $commands[] = ajax_command_html('#ajax_selected_lab', '');
		   $commands[] = ajax_command_html('#ajax_selected_lab_freeeda', ''); 
		   $commands[] = ajax_command_html('#ajax_selected_lab_pdf', '');
		   $commands[] = ajax_command_data('#ajax_selected_lab', 'form_state_value_select', $form_state['values']['lab']);
		   $commands[] = ajax_command_html('#ajax_selected_experiment','');
		   $commands[] = ajax_command_html('#ajax_download_experiments', '');
		   $commands[] = ajax_command_html('#ajax_selected_solution','');
		   $commands[] = ajax_command_html('#ajax_solution_files', '');
		   $commands[] = ajax_command_html('#ajax_download_experiment_solution', '');
		   $commands[] = ajax_command_html('#ajax_edit_experiment_solution', '');
      
      }
      
      return array('#type' => 'ajax', '#commands' => $commands);
    }

function ajax_solution_list_callback($form, $form_state){
		 
		   $commands = array();
		   $experiment_list_default_value = $form_state['values']['lab_experiment_list'];
		   //var_dump($lab_default_value);
		   if($experiment_list_default_value !=0){      
		   
		   $form['lab_solution_list']['#options'] = _ajax_get_solution_list($experiment_list_default_value);
		   
		   $commands[] = ajax_command_html('#ajax_download_experiments', l('Download Experiment', 'lab_migration/download/experiment/' . $experiment_list_default_value));
      
      	$commands[] = ajax_command_html('#ajax_selected_experiment',drupal_render($form['lab_experiment_list']));
      
     		$commands[] = ajax_command_html('#ajax_selected_solution',drupal_render($form['lab_solution_list']));

    // $commands[] = ajax_command_html('#ajax_solution_files', '');
      // $commands[] = ajax_command_html('#ajax_download_experiment_solution', '');
     // $commands[] = ajax_command_html('#ajax_edit_experiment_solution', '');
      
      }else
      {
		   $form['lab_solution_list']['#options'] = _ajax_get_solution_list();
		   $commands[] = ajax_command_html('#ajax_selected_solution',drupal_render($form['lab_solution_list']));

		   $commands[] = ajax_command_html('#ajax_download_experiments', '');
		   $commands[] = ajax_command_html('#ajax_selected_solution','');
		   $commands[] = ajax_command_html('#ajax_solution_files', '');
		   $commands[] = ajax_command_html('#ajax_download_experiment_solution', '');
		   $commands[] = ajax_command_html('#ajax_edit_experiment_solution', '');
    // $commands[] = ajax_command_replace('#ajax_selected_experiment',drupal_render($form['lab_experiment_list']));
      
      }
      
      return array('#type' => 'ajax', '#commands' => $commands);
		}
		
		function ajax_solution_files_callback($form, $form_state){
		$commands = array();
      $solution_list_default_value = $form_state['values']['lab_solution_list'];
      //var_dump($lab_default_value);
      if($solution_list_default_value !=0){
       $commands[] = ajax_command_html('#ajax_selected_lab_experiment_solution_action',drupal_render($form['lab_experiment_solution_actions']));
       /*************************************************************************************/
       //$solution_list_q = db_query("SELECT * FROM {lab_migration_solution_files} WHERE solution_id = %d", $form_state['values']['solution']);
		$query = db_select('lab_migration_solution_files');
               $query->fields('lab_migration_solution_files');
               $query->condition('solution_id', $solution_list_default_value);
      $solution_list_q = $query->execute();
      if ($solution_list_q)
      {
        $solution_files_rows = array();
        while ($solution_list_data = $solution_list_q->fetchObject())
        {
          $solution_file_type = '';
          switch ($solution_list_data->filetype)
          {
          case 'S' : $solution_file_type = 'Source or Main file'; break;
          case 'R' : $solution_file_type = 'Result file'; break;
          case 'X' : $solution_file_type = 'xcos file'; break;
          default  : $solution_file_type = 'Unknown'; break;
          }
          $solution_files_rows[] = array(l($solution_list_data->filename, 'lab_migration/download/file/' . $solution_list_data->id), $solution_file_type);
	if(strlen($solution_list_data->pdfpath)>=5){
	
	$pdfname=substr($solution_list_data->pdfpath, strrpos($solution_list_data->pdfpath, '/') + 1);
	$solution_files_rows[] = array(l($pdfname, 'lab_migration/download/pdf/' . $solution_list_data->id),"PDF File");
	}
        }

        /* dependency files */
        //$dependency_q = db_query("SELECT * FROM {lab_migration_solution_dependency} WHERE solution_id = %d", $form_state['values']['solution']);
        $query = db_select('lab_migration_solution_dependency');
                 $query->fields('lab_migration_solution_dependency');
                 $query->condition('solution_id', $solution_list_default_value);
        $dependency_q = $query->execute();
				while ($dependency_data = $dependency_q->fetchObject())
				{
					//$dependency_files_q = db_query("SELECT * FROM {lab_migration_dependency_files} WHERE id = %d", $dependency_data->dependency_id);
					$query = db_select('lab_migration_dependency_files');
								$query->fields('lab_migration_dependency_files');
								$query->condition('id', $dependency_data->dependency_id);
				   $dependency_files_q = $query->execute();
					$dependency_files_data = $dependency_files_q->fetchObject();
					$solution_file_type = 'Dependency file';
					$solution_files_rows[] = array(l($dependency_files_data->filename, 'lab_migration/download/dependency/' . $dependency_files_data->id), $solution_file_type);
				}

        /* creating list of files table */
        $solution_files_header = array('Filename', 'Type');
        $solution_files = theme('table', array('header' => $solution_files_header, 'rows' => $solution_files_rows)); 
       
      }
		 $form['solution_files']['#title'] = 'List of solution files';
		 $form['solution_files']['#markup'] = $solution_files;
       $commands[] = ajax_command_html('#ajax_download_experiment_solution', l('Download Solution', 'lab_migration/download/solution/' . $solution_list_default_value));
    //   $commands[] = ajax_command_html('#ajax_edit_experiment_solution', l('Edit Solution', 'code_approval/editcode/' . $solution_list_default_value));
      // $commands[] = ajax_command_html('#ajax_solution_files', $solution_files);
       $commands[] = ajax_command_html('#ajax_solution_files',drupal_render($form['solution_files']));
      }else{
		   $commands[] = ajax_command_html('#ajax_selected_lab_experiment_solution_action','');
		   $commands[] = ajax_command_html('#ajax_download_experiment_solution', '');
		   $commands[] = ajax_command_html('#ajax_edit_experiment_solution', '');
		   $commands[] = ajax_command_html('#ajax_solution_files', '');
      }
      
      return array('#type' => 'ajax', '#commands' => $commands);
		}
		
		function bootstrap_table_format($headers, $rows) {
        $thead = "";
        $tbody = "";
        foreach($headers as $header) {
            $thead .= "<th>{$header}</th>";
        }
        foreach($rows as $row) {
            $tbody .= "<tr>";
            foreach($row as $data) {
                $tbody .= "<td>{$data}</td>";
            }
            $tbody .= "</tr>";
        }
        $table = "
            <table class='table table-bordered table-hover' style='margin-left:-140px'>
                <thead>{$thead}</thead>
                <tbody>{$tbody}</tbody>
            </table>
        ";
        return $table;
    }
            
    /*****************************************************/
function _list_of_labs()
 {
  $lab_titles = array('0' => 'Please select...');
  //$lab_titles_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE solution_display = 1 ORDER BY lab_title ASC");
  $query = db_select('lab_migration_proposal');
              $query->fields('lab_migration_proposal');
              $query->condition('solution_display', 1);
	      $query->condition('approval_status', 3);
              $query->orderBy('lab_title', 'ASC');
              $lab_titles_q = $query->execute();
  while ($lab_titles_data = $lab_titles_q->fetchObject())
  {
    $lab_titles[$lab_titles_data->id] = $lab_titles_data->lab_title . ' (Proposed by ' . $lab_titles_data->name . ')';
  }
  return $lab_titles;
}

     function _ajax_get_experiment_list($lab_default_value = ''){
         $experiments = array('0' => 'Please select...');
  //$experiments_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE proposal_id = %d ORDER BY number ASC", $proposal_id);
  $query = db_select('lab_migration_experiment');
              $query->fields('lab_migration_experiment');
              $query->condition('proposal_id', $lab_default_value);
              $query->orderBy('number', 'ASC');
              $experiments_q = $query->execute();
  while ($experiments_data = $experiments_q->fetchObject())
  {
    $experiments[$experiments_data->id] = $experiments_data->number . '. ' . $experiments_data->title;
  }
  return $experiments;
    }

           
	function _ajax_get_solution_list($lab_experiment_list=''){
     	   $solutions = array('0' => 'Please select...');
 // $solutions_q = db_query("SELECT * FROM {lab_migration_solution} WHERE experiment_id = %d ORDER BY
  //  CAST(SUBSTRING_INDEX(code_number, '.', 1) AS BINARY) ASC,
 //   CAST(SUBSTRING_INDEX(SUBSTRING_INDEX(code_number , '.', 2), '.', -1) AS UNSIGNED) ASC,
  //  CAST(SUBSTRING_INDEX(SUBSTRING_INDEX(code_number , '.', -1), '.', 1) AS UNSIGNED) ASC", $experiment_id);

  $query = db_select('lab_migration_solution');
                $query->fields('lab_migration_solution');
                $query->condition('experiment_id', $lab_experiment_list);
                //$query->orderBy("CAST(SUBSTRING_INDEX(code_number, '.', 1) AS BINARY", "ASC");
               // $query->orderBy("CAST(SUBSTRING_INDEX(SUBSTRING_INDEX(code_number , '.', 2), '.', -1) AS UNSIGNED", "ASC");
               // $query->orderBy("CAST(SUBSTRING_INDEX(SUBSTRING_INDEX(code_number , '.', -1), '.', 1) AS UNSIGNED", "ASC");
                $solutions_q = $query->execute();
  while ($solutions_data = $solutions_q->fetchObject())
  {
    $solutions[$solutions_data->id] = $solutions_data->code_number . ' (' . $solutions_data->caption . ')';
  }
  return $solutions;
   }
     
   function _lab_information($proposal_id)
{
	//$lab_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);
  $query = db_select('lab_migration_proposal');
              $query->fields('lab_migration_proposal');
              $query->condition('id', $proposal_id);
              $lab_q = $query->execute();
  $lab_data = $lab_q->fetchObject();
  return $lab_data;
}       
     
     
 function _lab_details($lab_default_value){
 //$lab_default_value = $form_state['values']['lab'];
		  $lab_details = _lab_information($lab_default_value);
		  if($lab_default_value !=0){
	if ($lab_details->solution_provider_uid > 0) {
    	$user_solution_provider = user_load($lab_details->solution_provider_uid);
    	if ($user_solution_provider) {
    		$solution_provider = '<span style="color: rgb(128, 0, 0);"><strong>Solution Provider</strong></span></td><td style="width: 35%;"><br />' .
          '<ul>' .
          '<li><strong>Solution Provider Name:</strong> ' . $lab_details->solution_provider_name_title . ' ' . $lab_details->solution_provider_name . '</li>' .
          '<li><strong>Department:</strong> ' . $lab_details->solution_provider_department . '</li>' .
          '<li><strong>University:</strong> ' . $lab_details->solution_provider_university . '</li>' .
          '</ul>';
          
         
    	} else {
    		$solution_provider = '<span style="color: rgb(128, 0, 0);"><strong>Solution Provider</strong></span></td><td style="width: 35%;"><br />' .
					'<ul>' .
					'<li><strong>Solution Provider: </strong> (Open) </li>' .
					'</ul>';
    	}
    } else {
    	$solution_provider = '<span style="color: rgb(128, 0, 0);"><strong>Solution Provider</strong></span></td><td style="width: 35%;"><br />' .
        '<ul>' .
        '<li><strong>Solution Provider: </strong> (Open) </li>' .
        '</ul>';
    }
          }
$form['lab_details']['#markup']= '<span style="color: rgb(128, 0, 0);"><strong>About the Lab</strong></span></td><td style="width: 35%;"><br />' .
        '<ul>' .
        '<li><strong>Proposer Name:</strong> ' . $lab_details->name_title . ' ' . $lab_details->name . '</li>' .
        '<li><strong>Title of the Lab:</strong> ' . $lab_details->lab_title . '</li>' .
        '<li><strong>Department:</strong> ' . $lab_details->department . '</li>' .
        '<li><strong>University:</strong> ' . $lab_details->university . '</li>' .
        '<li><strong>Category:</strong> ' .  $category_data . '</li>' .
        '</ul>' .
        $solution_provider;
        
        $details = $form['lab_details']['#markup'];
        return $details;
 }  
 
