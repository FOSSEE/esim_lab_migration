<?php
// $Id$

/*
	Approval Status :
	0 - Pending
	1 - Approved
	2 - Dis-Approved
	3 - Solved
  Solution Status :
	0 - Pending
	1 - Approved
	2 - Dis-Approved
  Solution Display :
  0 - No
  1 - Yes

  Tables :
  lab_migration_solution : approval_status
  	0 - Pending
  	1 - Approved
  	2 - Disapproved (delete it)
*/

function lab_migration_proposal_form($form_state)
{
  global $user;

  /************************ start approve book details ************************/  
  $query = db_select('lab_migration_proposal');
			$query->fields('lab_migration_proposal');
			$query->condition('uid', $user->uid);
			$query->orderBy('id', 'DESC');
			$query->range(0, 1);
			$proposal_q = $query->execute();
  $proposal_data = $proposal_q->fetchObject();
  if ($proposal_data)
  {
		if ($proposal_data->approval_status == 0 || $proposal_data->approval_status == 1) {
			drupal_set_message(t('We have already received your proposal.'), 'status');
			drupal_goto('');
			return;
		}
  }


  $form['#attributes'] = array('enctype' => "multipart/form-data");

  $form['name_title'] = array(
    '#type' => 'select',
    '#title' => t('Title'),
    '#options' => array('Dr' => 'Dr', 'Prof' => 'Prof'),
    '#required' => TRUE,
  );
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name of the Proposer'),
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
  );
  $form['email_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Email'),
    '#size' => 30,
    '#value' => $user->mail,
    '#disabled' => TRUE,
  );
  $form['contact_ph'] = array(
    '#type' => 'textfield',
    '#title' => t('Contact No.'),
    '#size' => 30,
    '#maxlength' => 15,
    '#required' => TRUE,
  );
  $form['department'] = array(
    '#type' => 'select',
    '#title' => t('Department/Branch'),
    '#options' => _lm_list_of_departments(),
    '#required' => TRUE,
  );
  $form['university'] = array(
    '#type' => 'textfield',
    '#title' => t('University/ Institute'),
    '#size' => 80,
    '#maxlength' => 200,
    '#required' => TRUE,
    '#attributes' =>array('placeholder' =>'Insert full name of your institute/ university.... '),
  );
  $form['country'] = array(
    '#type' => 'select',    
    '#title' => t('Country'),
    '#options' => array(
                'India' => 'India',
                'Others' => 'Others',
                ),
    '#required' => TRUE,
    '#tree' => TRUE,
    '#validated' => TRUE,
  );
  $form['other_country'] = array(
    '#type' => 'textfield',
    '#title' => t('Other than India'),
    '#size' => 100,
    '#attributes' =>array('placeholder' => t('Enter your country name')),
    '#states' => array(
      'visible' => array(
        ':input[name="country"]' => array('value' => 'Others'),
      ),
    ),
  );
  $form['other_state'] = array(
    '#type' => 'textfield',
    '#title' => t('State other than India'),
    '#size' => 100,
    '#attributes' =>array('placeholder' => t('Enter your state/region name')),
    '#states' => array(
      'visible' => array(
        ':input[name="country"]' => array('value' => 'Others'),
      ),
    ),
  );
  $form['other_city'] = array(
    '#type' => 'textfield',
    '#title' => t('City other than India'),
    '#size' => 100,
    '#attributes' =>array('placeholder' => t('Enter your city name')),
    '#states' => array(
      'visible' => array(
        ':input[name="country"]' => array('value' => 'Others'),
      ),
    ),
  );  
  $form['all_state'] = array(
    '#type' => 'select',    
    '#title' => t('State'),
    '#options' => _lm_list_of_states(),
    '#validated' => TRUE,
    '#states' => array(
      'visible' => array(
        ':input[name="country"]' => array('value' => 'India'),
      ),
    ),
  ); 
  $form['city'] = array(
    '#type' => 'select',
    '#title' => t('City'),
    '#options' => _lm_list_of_cities(),    
    '#states' => array(
      'visible' => array(
        ':input[name="country"]' => array('value' => 'India'),
      ),
    ),
  ); 
  $form['pincode'] = array(
    '#type' => 'textfield',
    '#title' => t('Pincode'),
    '#size' => 30,
    '#maxlength' => 6,
    '#required' => False,
    '#attributes' =>array('placeholder' =>'Enter pincode....'),
  );
 /***************************************************************************/
  $form['hr'] = array(
    '#type' => 'item',
    '#markup' => '<hr>',
  );
 

   $form['esim_version'] = array(
    '#type' => 'select',
    '#title' => t('esim version used'),
    '#options' => _lm_list_of_esim_version(),
    '#required' => TRUE,
  );
  $form['lab_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title of the Lab'),
    '#size' => 50,
    '#required' => TRUE,
  );

  $first_experiemnt = TRUE;
  for ($counter = 1; $counter <= 15; $counter++) {

		if($counter<=5){
		        $form['lab_experiment-' . $counter] = array(
			'#type' => 'textfield',
			'#title' => t('Title of the Experiment ') . $counter,
			'#size' => 50,
			'#required' => TRUE,
			);
			$namefield="lab_experiment-" . $counter;
			$form['lab_experiment_description-' . $counter] = array(
			'#type' => 'textarea',
			'#required' => TRUE,
			'#attributes' =>array('placeholder' => t('Enter Description for your experiment '.$counter.' ( Note: Minimum 50 characters required)'),'cols' => 50, 'rows' => 4),
			'#title' => t('Description for Experiment ') . $counter,
			'#settings' => array(
    				'max_length' => 50,
  			 ),
			'#states' => array(
            		'invisible' => array(
                	':input[name='.$namefield.']' => array('value' => ""),
           		 ),
       			 ),
			);
                   }else{
			 $form['lab_experiment-' . $counter] = array(
			'#type' => 'textfield',
			'#title' => t('Title of the Experiment ') . $counter,
			'#size' => 50,
			'#required' => FALSE,
			);
			$namefield="lab_experiment-" . $counter;
			$form['lab_experiment_description-' . $counter] = array(
			'#type' => 'textarea',
			'#required' => FALSE,
			'#attributes' =>array('placeholder' => t('Enter Description for your experiment '.$counter.' ( Note: Minimum 50 characters required)'),'cols' => 50, 'rows' => 4),
			'#title' => t('Description for Experiment ') . $counter,
			'#states' => array(
            		'invisible' => array(
                	':input[name='.$namefield.']' => array('value' => ""),
           		 ),
       			 ),
			);
			}
			$first_experiemnt = FALSE;	
		       
	}

  $form['solution_provider_uid'] = array(
	  '#type' => 'radios',
		'#title' => t('Do you want to provide the solution'),
		'#options' => array('1' => 'Yes', '2' => 'No'),
		'#required' => TRUE,
		'#default_value' => '1',
		'#description' => 'If you dont want to provide the solution then it will be opened for the community, anyone may come forward and provide the solution.',
  );

  $form['solution_display'] = array(
	  '#type' => 'hidden',
		'#title' => t('Do you want to display the solution on the www.esim.fossee.in website'),
		'#options' => array('1' => 'Yes'),
		'#required' => TRUE,
		'#default_value' => '1',
		'#description' => 'If yes, solutions will be made available to everyone for downloading.',
        '#disabled' => FALSE,
  );

  $form['samplefile'] = array(
    '#type' => 'fieldset',
    '#title' => t('Sample Source File'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
	'#states' => array(
            		'visible' => array(
                	':input[name="solution_provider_uid"]' => array('value' =>1),
           		 ),
       			 ),
  );
  $form['samplefile']['samplefile1'] = array(
      '#type' => 'file',
      '#title' => t('Upload sample source file'),
      '#size' => 48,
      '#description' => t('Separate filenames with underscore. No spaces or any special characters allowed in filename.') . '<br />' .
      t('<span style="color:red;">Allowed file extensions : ') . variable_get('textbook_companion_source_extensions', '').'</span>',
     
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );
  return $form;
}

function lab_migration_proposal_form_validate($form, &$form_state)
{
  if (!preg_match('/^[0-9\ \+]{0,15}$/', $form_state['values']['contact_ph'])){
    form_set_error('contact_ph', t('Invalid contact phone number'));
 }

  for ($counter = 1; $counter <= 15; $counter++) {
		if($counter<=5){
		$experiment_field_name = 'lab_experiment-' . $counter;
		$experiment_description='lab_experiment_description-' . $counter;
		if (strlen(trim($form_state['values'][$experiment_field_name])) <= 1) {
		
			 form_set_error($experiment_field_name, t('Minimum 5 Experiment are compulsory for proposal'));
		
	   		}
		}
	} 
 if($form_state['values']['country']=='Others'){
         if($form_state['values']['other_country']== ''){
            form_set_error('other_country', t('Enter country name'));
           // $form_state['values']['country'] = $form_state['values']['other_country'];
         }else{
         $form_state['values']['country'] = $form_state['values']['other_country'];
         }        
         if($form_state['values']['other_state']== ''){
            form_set_error('other_state', t('Enter state name'));
           // $form_state['values']['country'] = $form_state['values']['other_country'];
         }else{
         $form_state['values']['all_state'] = $form_state['values']['other_state'];
         }
         if($form_state['values']['other_city']== ''){
            form_set_error('other_city', t('Enter city name'));
           // $form_state['values']['country'] = $form_state['values']['other_country'];
         }else{
         $form_state['values']['city'] = $form_state['values']['other_city'];
         }
 }else{
         if($form_state['values']['country']== ''){
            form_set_error('country', t('Select country name'));
           // $form_state['values']['country'] = $form_state['values']['other_country'];
         }
         if($form_state['values']['all_state']== ''){
            form_set_error('all_state', t('Select state name'));
           // $form_state['values']['country'] = $form_state['values']['other_country'];
         }
         if($form_state['values']['city']== ''){
            form_set_error('city', t('Select city name'));
           // $form_state['values']['country'] = $form_state['values']['other_country'];
         }
         
 
 }




	for ($counter = 1; $counter <= 15; $counter++) {
		$experiment_field_name = 'lab_experiment-' . $counter;
		$experiment_description='lab_experiment_description-' . $counter;
		if (strlen(trim($form_state['values'][$experiment_field_name])) >= 1) {
		if (strlen(trim($form_state['values'][$experiment_description])) <=49) {
			 form_set_error($experiment_description, t('Description should be minimum of 50 characters'));
		}
	    }
	}

  if($form_state['values']['solution_provider_uid']==1){

 if (isset($_FILES['files']))
  {
    /* check if atleast one source or result file is uploaded */  
    if ( ! ($_FILES['files']['name']['samplefile1']))
      form_set_error('samplefile1', t('Please upload sample code main or source file.'));

    /* check for valid filename extensions */  
    foreach ($_FILES['files']['name'] as $file_form_name => $file_name)
    {
      if ($file_name)
      {
        /* checking file type */
        if (strstr($file_form_name, 'sample'))
          $file_type = 'S';
        else
          $file_type = 'U';

        $allowed_extensions_str = '';
        switch ($file_type)
        {
        case 'S':
          $allowed_extensions_str = variable_get('lab_migration_source_extensions', '');
          break;
	
        }
        $allowed_extensions = explode(',' , $allowed_extensions_str);
        $temp_extension = end(explode('.', strtolower($_FILES['files']['name'][$file_form_name])));
        if (!in_array($temp_extension, $allowed_extensions))
          form_set_error($file_form_name, t('Only file with ' . $allowed_extensions_str . ' extensions can be uploaded.'));
        if ($_FILES['files']['size'][$file_form_name] <= 0)
          form_set_error($file_form_name, t('File size cannot be zero.'));

        /* check if valid file name */
        if (!lab_migration_check_valid_filename($_FILES['files']['name'][$file_form_name]))
         form_set_error($file_form_name, t('Invalid file name specified. Only alphabets and numbers are allowed as a valid filename.'));
      }
    }
  }
}
  return;

}

function lab_migration_proposal_form_submit($form, &$form_state)
{
  global $user;
  $root_path =lab_migration_samplecode_path();
  if (!$user->uid) {
		drupal_set_message('It is mandatory to login on this website to access the proposal form', 'error');
		return;
  }

  $solution_provider_uid = 0;
  $solution_status = 0;
  $solution_provider_name_title = '';
  $solution_provider_name = '';
  $solution_provider_contact_ph = '';
  $solution_provider_department = '';
  $solution_provider_university = '';
  if ($form_state['values']['solution_provider_uid'] == "1") {
  	$solution_provider_uid = $user->uid;
  	$solution_status = 1;
		$solution_provider_name_title = $form_state['values']['name_title'];
		$solution_provider_name = $form_state['values']['name'];
		$solution_provider_contact_ph = $form_state['values']['contact_ph'];
		$solution_provider_department = $form_state['values']['department'];
		$solution_provider_university = $form_state['values']['university'];
  } else {
  	$solution_provider_uid = 0;
  }

  $solution_display = 0;
  if ($form_state['values']['solution_display'] == "1")
  	$solution_display = 1;
  else
  	$solution_display = 1;

  /* inserting the user proposal */
    $v = $form_state["values"];
  $result = "INSERT INTO {lab_migration_proposal} 
    (uid, approver_uid, name_title, name, contact_ph, department, university, city, pincode, state,country,esim_version, lab_title, approval_status, solution_status, solution_provider_uid, solution_display, creation_date, approval_date, solution_date, solution_provider_name_title, solution_provider_name, solution_provider_contact_ph, solution_provider_department, solution_provider_university,samplefilepath) VALUES
    (:uid, :approver_uid, :name_title, :name, :contact_ph, :department, :university, :city, :pincode, :state, :country,:esim_version,:lab_title,:approval_status,
 :solution_status, :solution_provider_uid, :solution_display, :creation_date, :approval_date, :solution_date, 
:solution_provider_name_title, :solution_provider_name, :solution_provider_contact_ph, :solution_provider_department, 
:solution_provider_university,:samplefilepath)";
  $args= array(
    ":uid" => $user->uid,
    ":approver_uid" => 0,
    ":name_title" => $v['name_title'],
    ":name" => $v['name'],
    ":contact_ph" => $v['contact_ph'],
    ":department" => $v['department'],
    ":university" => $v['university'],
    ":city" => $v['city'],
    ":pincode" => $v['pincode'],
    ":state" => $v['all_state'],
    ":country" => $v['country'],
    ":lab_title" => $v['lab_title'],
    ":esim_version"=>$v['esim_version'],
    ":approval_status" =>  0,
    ":solution_status" => $solution_status,
    ":solution_provider_uid" => $solution_provider_uid,
    ":solution_display" => $solution_display,
    ":creation_date" => time(),
    ":approval_date" => 0,
    ":solution_date" => 0,
    ":solution_provider_name_title" => $solution_provider_name_title,
    ":solution_provider_name" => $solution_provider_name,
    ":solution_provider_contact_ph" => $solution_provider_contact_ph,
    ":solution_provider_department" => $solution_provider_department,
    ":solution_provider_university" => $solution_provider_university,
    ":samplefilepath"=>""
  );
  $proposal_id = db_query($result, $args, array('return' => Database::RETURN_INSERT_ID)); 

 if (!$proposal_id)
  {
    drupal_set_message(t('Error receiving your proposal. Please try again.'), 'error');
    return;
  }

if ($form_state['values']['solution_provider_uid'] == "1") {
  $dest_path = $proposal_id . '/';
  if (!is_dir($root_path . $dest_path))
    mkdir($root_path . $dest_path);


      /* uploading files */
  foreach ($_FILES['files']['name'] as $file_form_name => $file_name)
  {
    if ($file_name)
    {
       /* checking file type */
      $file_type = 'S';

      if (file_exists($root_path . $dest_path . $_FILES['files']['name'][$file_form_name]))
      {
        drupal_set_message(t("Error uploading file. File !filename already exists.", array('!filename' => $_FILES['files']['name'][$file_form_name])), 'error');
        return;
      }

      /* uploading file */
      if (move_uploaded_file($_FILES['files']['tmp_name'][$file_form_name], $root_path . $dest_path . $_FILES['files']['name'][$file_form_name]))
      {
       $query = "UPDATE {lab_migration_proposal} SET samplefilepath = :samplefilepath WHERE id = :id";
     $args = array(
     ":samplefilepath" => $dest_path . $_FILES['files']['name'][$file_form_name], 
     ":id" =>$proposal_id,
     );
    $updateresult = db_query($query, $args);

        drupal_set_message($file_name . ' uploaded successfully.', 'status');
      } else {
        drupal_set_message('Error uploading file : ' . $dest_path . '/' . $file_name, 'error');
      }
    }
  }
}

  /* proposal id */
  //$proposal_id = db_last_insert_id('lab_migration_proposal', 'id');

  /* adding experiments */
  $number = 1;
  for ($counter = 1; $counter <= 15; $counter++) {
  	$experiment_field_name = 'lab_experiment-' . $counter;
	$experiment_description='lab_experiment_description-' . $counter;
  	if (strlen(trim($form_state['values'][$experiment_field_name])) >= 1) {
        $query = "INSERT INTO {lab_migration_experiment} (proposal_id, number, title,description) VALUES (:proposal_id, :number, :experiment_field_name,:description)";
        $args= array(
            ":proposal_id" => $proposal_id, 
            ":number" => $number, 
            ":experiment_field_name"=> trim($form_state['values'][$experiment_field_name]),
	    ":description"=>trim($form_state['values'][$experiment_description])
        );
        $result = db_query($query, $args);
			if (!$result)
			{
				drupal_set_message(t('Could not insert Title of the Experiment : ') . trim($form_state['values'][$experiment_field_name]), 'error');
			} else {
				$number++;
			}
  	}
	}

  /* sending email */
  $email_to = $user->mail;
	
  $from = variable_get('lab_migration_from_email', '');
  $bcc= variable_get('lab_migration_emails', '');
  $cc=variable_get('lab_migration_cc_emails', '');

  $param['proposal_received']['proposal_id'] = $proposal_id;
  $param['proposal_received']['user_id'] = $user->uid;
  $param['proposal_received']['headers']=array('From'=>$from,'MIME-Version'=> '1.0',
    			'Content-Type'=> 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
    			'Content-Transfer-Encoding' => '8Bit',
    			'X-Mailer'=> 'Drupal','Cc' => $cc, 'Bcc' => $bcc);

  if (!drupal_mail('lab_migration', 'proposal_received', $email_to , user_preferred_language($user), $param, $from, TRUE))
    drupal_set_message('Error sending email message.', 'error');

  /* sending email */
  /*$email_to = variable_get('lab_migration_emails', '');
  if (!drupal_mail('lab_migration', 'proposal_received', $email_to , user_preferred_language($user), $param, variable_get('lab_migration_from_email', NULL), TRUE))
    drupal_set_message('Error sending email message.', 'error');*/

  drupal_set_message(t('We have received you Lab migration proposal. We will get back to you soon.'), 'status');
  drupal_goto('');
}




function _lm_list_of_states(){
			$states = array(0 => '-Select-');  
			$query = db_select('list_states_of_india');
				$query->fields('list_states_of_india');
				
				//$query->orderBy('', '');
				$states_list = $query->execute();
			while ($states_list_data = $states_list->fetchObject())
			{
				$states[$states_list_data->state] = $states_list_data->state;
			}
			return $states;
}
function _lm_list_of_cities(){
			$city = array(0 => '-Select-');  
			$query = db_select('list_cities_of_india');
				$query->fields('list_cities_of_india');
				$query->orderBy('city', 'ASC');
				$city_list = $query->execute();
			while ($city_list_data = $city_list->fetchObject())
			{
				$city[$city_list_data->city] = $city_list_data->city;
			}
			return $city;
}
function _lm_list_of_departments(){
			$department = array(0 => '-Select-');  
			$query = db_select('list_of_departments');
				$query->fields('list_of_departments');
				$query->orderBy('id', 'DESC');
				$department_list = $query->execute();
			while ($department_list_data = $department_list->fetchObject())
			{
				$department[$department_list_data->department] = $department_list_data->department;
			}
			return $department;
}
function _lm_list_of_esim_version(){
			$esim_version = array();  
			$query = db_select('esim_software_version');
				$query->fields('esim_software_version');
				$query->orderBy('id', 'ASC');
				$version_list = $query->execute();
			while ($version_list_data = $version_list->fetchObject())
			{
				$esim_version[$version_list_data->esim_version] =$version_list_data->esim_version;
			}
			return $esim_version;
}

