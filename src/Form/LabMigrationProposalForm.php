<?php

/**
 * @file
 * Contains \Drupal\lab_migration\Form\LabMigrationProposalForm.
 */

namespace Drupal\lab_migration\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Render\Element;
use Drupal\Core\Link;
use Drupal\Core\Url;
use Drupal\Core\Routing\TrustedRedirectResponse;
use Symfony\Component\HttpFoundation\Response;
use Symfony\Component\HttpFoundation\RedirectResponse;
use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\Core\StringTranslation\StringTranslationTrait;
use Drupal\Core\Database\Database;
use Drupal\Core\Messenger\MessengerInterface;
use Drupal\Core\Mail\MailManager;
use Drupal\Core\Mail\MailManagerInterface;
use Drupal\Core\DependencyInjection\ContainerInterface;
use Drupal\Core\Session\AccountProxyInterface;
use Drupal\user\Entity\User;

class LabMigrationProposalForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'lab_migration_proposal_form';
  }

  public function buildForm(array $form, FormStateInterface $form_state) {
    $user = \Drupal::currentUser();
    $form = [];
    $state = \Drupal::service("lab_migration_global")->_lab_migration_list_of_states();
    $selected_state = $form_state->getValue('all_state') ?: key($state);
    // $selected_state = !$form_state->getValue(['all_state']) ? $form_state->getValue([
      // 'all_state'
      // :key($state);
      $district = \Drupal::service("lab_migration_global")->_lab_migration_list_of_district();
      $selected_district = $form_state->getValue('district') ?: key($district); 
      // $selected_district = !$form_state->getValue(['district']) ? $form_state->getValue([
      // 'district'
      // ]):key($district);
      $city = \Drupal::service("lab_migration_global")->_lm_list_of_cities();
      $selected_city = $form_state->getValue('city') ?: key($city);
    // $selected_city = !$form_state->getValue(['city']) ? $form_state->getValue([
      // 'city'
      // ]):key($city);
      $pincode = \Drupal::service("lab_migration_global")->_lab_migration_list_of_city_pincode();
      $selected_pincode= $form_state->getValue('pincode') ?: key($pincode);
      // $selected_pincode = !$form_state->getValue(['picode'])?$form_state->getValue([
      // 'pincode'
      // ]):key($pincode);
    /************************ start approve book details ************************/
    if ($user->isAnonymous()) {
      // $msg = \Drupal::messenger()->addError(t('This is an error message, red in color'));
      $url = Link::fromTextAndUrl(t('login'), Url::fromRoute('user.page'))->toString();
      
      $msg = \Drupal::messenger()->addmessage(t('It is mandatory to ' . Link::fromTextAndUrl(t('login'), Url::fromRoute('user.page'))->toString() . ' on this website to access the lab proposal form. If you are new user please create a new account first.'));
      
      // RedirectResponse('lab-migration-project');
      // \Drupal::RedirectResponse('user');
  //     $redirect = new RedirectResponse($url);
  //     $redirect->send();
  // return $msg;
  // Redirect to the login page
  $response = new RedirectResponse(Url::fromRoute('user.page')->toString());

  $response->send();
  return $msg;

  // /lab-migration/proposal
    }
    $query = \Drupal::database()->select('lab_migration_proposal');
    $query->fields('lab_migration_proposal');
    $query->condition('uid', $user->id());
    $query->orderBy('id', 'DESC');
    $query->range(0, 1);
    $proposal_q = $query->execute();
    $proposal_data = $proposal_q->fetchObject();
    if ($proposal_data) {
      if ($proposal_data->approval_status == 0 || $proposal_data->approval_status == 1) {
         \Drupal::messenger()->addmessage(t('We have already received your proposal.'));
        // Create a redirect response to the front page
$response = new RedirectResponse(Url::fromRoute('<front>')->toString());

// Send the redirect response
//$response->send();

      return $response;
      }
    }
    $form['#attributes'] = ['enctype' => "multipart/form-data"];
    $form['name_title'] = [
      '#type' => 'select',
      '#attributes' => array('class' => array('form-control')),
'#title' => $this->t('Title'),
      '#options' => [
        'Dr' => 'Dr',
        'Prof' => 'Prof',
        
      ],
      '#required' => TRUE,
    ];
    $form['name'] = [
      '#type' => 'textfield',
      // '#attributes' => array('class' => array('form-control')),
'#title' => $this->t('Name of the Proposer'),
      '#size' => 100,
      '#attributes' => [
  'class' => ['form-control'],
        'placeholder' => $this->t('Enter your full name')
        ],
      '#maxlength' => 200,
      '#required' => TRUE,
    ];
    $form['email_id'] = [
      '#type' => 'textfield',
      // '#attributes' => array('class' => array('form-control')),
'#title' => $this->t('Email'),
      '#size' => 30,
      '#value' => $user ? $user->getEmail() : '',
      '#disabled' => TRUE,
    ];
    $form['contact_ph'] = [
      '#type' => 'textfield',
      '#attributes' => array('class' => array('form-control')),
'#title' => $this->t('Contact No.'),
      '#size' => 30,
      '#attributes' => [
        'placeholder' => t('Enter your contact number')
        ],
      '#maxlength' => 15,
      '#required' => TRUE,
    ];
    $form['department'] = [
      '#type' => 'select',
      // '#attributes' => array('class' => array('form-control')),
'#title' => $this->t('Department/Branch'),
      '#options' => \Drupal::service("lab_migration_global")->_lm_list_of_departments(),
      '#required' => TRUE,
      
    ];
    $form['university'] = [
      '#type' => 'textfield',
      // '#attributes' => array('class' => array('form-control')),
'#title' => $this->t('University/ Institute'),
      '#size' => 50,
      '#maxlength' => 200,
      '#required' => TRUE,
      '#attributes' => [
        'placeholder' => 'Insert full name of your institute/ university.... '
        ],
    ];
    $form['country'] = [
      '#type' => 'select',
      // '#attributes' => array('class' => array('form-control')),
'#title' => $this->t('Country'),
      '#options' => [
        'India' => 'India',
        'Others' => 'Others',
      ],
      '#required' => TRUE,
      // '#attributes' => ['class' => ['form-control']],
      '#tree' => TRUE,
      '#validated' => TRUE,
    ];
    $form['other_country'] = [
      '#type' => 'textfield',
      // '#attributes' => array('class' => array('form-control')),
'#title' => $this->t('Other than India'),
      '#size' => 30,
      '#attributes' => [
        'placeholder' => $this->t('Enter your country name')
        ],
      '#states' => [
        'visible' => [
          ':input[name="country"]' => [
            'value' => 'Others'
            ]
          ]
        ],
    ];
    $form['other_state'] = [
      '#type' => 'textfield',
      // '#attributes' => array('class' => array('form-control')),
'#title' => t('State other than India'),
      '#size' => 50,
      '#attributes' => [
        'placeholder' => t('Enter your state/region name')
        ],
      '#states' => [
        'visible' => [
          ':input[name="country"]' => [
            'value' => 'Others'
            ]
          ]
        ],
    ];
    $form['other_city'] = [
      '#type' => 'textfield',
      // '#attributes' => array('class' => array('form-control')),
'#title' => $this->t('City other than India'),
      '#size' => 50,
      '#attributes' => [
        'placeholder' => $this->t('Enter your city name')
        ],
      '#states' => [
        'visible' => [
          ':input[name="country"]' => [
            'value' => 'Others'
            ]
          ]
        ],
    ];
    $form['all_state'] = [
      '#type' => 'select',
      '#attributes' => array('class' => array('form-control')),
'#title' => t('State'),
      '#options' => \Drupal::service("lab_migration_global")->_lm_list_of_states(),
      '#validated' => TRUE,
      '#states' => [
        'visible' => [
          ':input[name="country"]' => [
            'value' => 'India'
            ]
          ]
        ],
    ];
    $form['city'] = [
      '#type' => 'select',
      '#attributes' => array('class' => array('form-control')),
'#title' => t('City'),
      '#options' => \Drupal::service("lab_migration_global")->_lm_list_of_cities(),
      '#states' => [
        'visible' => [
          ':input[name="country"]' => [
            'value' => 'India'
            ]
          ]
        ],
    ];
    $form['pincode'] = [
      '#type' => 'textfield',
      '#attributes' => array('class' => array('form-control')),
'#title' => t('Pincode'),
      '#size' => 30,
      '#maxlength' => 6,
      '#required' => TRUE,
      '#attributes' => [
        'placeholder' => 'Enter pincode....'
        ],
    ];
    
    /***************************************************************************/
    $form['hr'] = [
      '#type' => 'item',
      '#markup' => '<hr>',
    ];
//     $form['operating_system'] = [
//       '#type' => 'textfield',
//       // '#attributes' => array('class' => array('form-control')),
// '#title' => t('Operating System'),
//       '#size' => 30,
//       '#maxlength' => 100,
//       '#required' => TRUE,
//     ];
    $form['version'] = [
      '#type' => 'select',
      // '#attributes' => array('class' => array('form-control')),
'#title' => $this->t('esim version '),
      '#options' =>\Drupal::service("lab_migration_global")->_lm_list_of_software_version(),
      '#required' => TRUE,
    ];
   
    $form['older'] = [
      '#type' => 'textfield',
      '#size' => 30,
      '#maxlength' => 50,
      //'#required' => TRUE,
        '#description' => $this->t('Specify the Older version used'),
      '#states' => [
        'visible' => [
          ':input[name="version"]' => [
            'value' => 'Others'
            ]
          ]
        ],
    ];

    $form['lab_title'] = [
      '#type' => 'textfield',
      '#attributes' => array('class' => array('form-control')),
'#title' => $this->t('Title of the Lab'),
      '#size' => 100,
      '#required' => TRUE,
    ];
    $first_experiemnt = TRUE;
    for ($counter = 1; $counter <= 15; $counter++) {
      if ($counter <= 1) {
        $form['lab_experiment-' . $counter] = [
          '#type' => 'textfield',
          '#attributes' => array('class' => array('form-control')),
'#title' => $this->t('Title of the Experiment ') . $counter,
          '#size' => 100,
          '#required' => TRUE,
        ];
        $namefield = "lab_experiment-" . $counter;
        $form['lab_experiment_description-' . $counter] = [
          '#type' => 'textarea',
          '#required' => TRUE,
          '#attributes' => [
            'placeholder' => $this->t('Enter Description for your experiment ' . $counter),
            'cols' => 50,
            'rows' => 4,
          ],
          '#attributes' => array('class' => array('form-control')),
'#title' => t('Description for Experiment ') . $counter,
          '#states' => [
            'invisible' => [
              ':input[name=' . $namefield . ']' => [
                'value' => ""
                ]
              ]
            ],
        ];
      }
      else {
        $form['lab_experiment-' . $counter] = [
          '#type' => 'textfield',
          '#attributes' => array('class' => array('form-control')),
'#title' => $this->t('Title of the Experiment ') . $counter,
          '#size' => 100,
          '#required' => FALSE,
        ];
        $namefield = "lab_experiment-" . $counter;
        $form['lab_experiment_description-' . $counter] = [
          '#type' => 'textarea',
          '#required' => FALSE,
          '#attributes' => [
            'placeholder' => $this->t('Enter Description for your experiment ' . $counter),
            'cols' => 50,
            'rows' => 4,
          ],
          '#attributes' => array('class' => array('form-control')),
'#title' => $this->t('Description for Experiment ') . $counter,
          '#states' => [
            'invisible' => [
              ':input[name=' . $namefield . ']' => [
                'value' => ""
                ]
              ]
            ],
        ];
      }
      $first_experiemnt = FALSE;
    }
    $form['solution_provider_uid'] = [
      '#type' => 'radios',
// '#attributes' => ['class' => ['form-control']],
'#title' => $this->t('Do you want to provide the solution'),
      '#options' => [
        '1' => 'Yes',
        '2' => 'No',
      ],
      '#required' => TRUE,
      '#default_value' => '1',
      '#description' => 'If you dont want to provide the solution then it will be opened for the community, anyone may come forward and provide the solution.',
    ];
    $form['solution_display'] = [
      '#type' => 'hidden',
      // '#attributes' => array('class' => array('form-control')),
'#title' => $this->t('Do you want to display the solution on the www.r.fossee.in website'),
      '#options' => [
        '1' => 'Yes'
        ],
      '#required' => TRUE,
      '#default_value' => '1',
      '#description' => 'If yes, solutions will be made available to everyone for downloading.',
      '#disabled' => FALSE,
    ];
    $form['samplefile'] = [
      '#type' => 'fieldset',
      '#title' => $this->t('Sample Source File'),
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,
      '#states' => [
        'visible' => [
          ':input[name="solution_provider_uid"]' => ['value' => 1],
        ],
      ],
    ];
    
    $form['samplefile']['samplefile1'] = [
      '#type' => 'file',
      '#title' => $this->t('Upload sample source file'),
      '#size' => 48,
      '#description' => $this->t('Separate filenames with underscore. No spaces or any special characters allowed in filename.') . '<br />' .
        // $this->t('<span style="color:red;">Allowed file extensions: ') .
      

        // \Drupal::config('textbook_companion.settings')->get('source_file_extensions') . '</span>',
       $this-> t('<span style="color:red;">Allowed file extensions : ') . \Drupal::config('lab_migration.settings')->get('lab_migration_source_extensions', '') . '</span>',
    ];
    
    $form['submit'] = [
      '#type' => 'submit',
      '#value' => $this->t('Submit'),
    ];
    return $form;
  }

  public function validateForm(array &$form, \Drupal\Core\Form\FormStateInterface $form_state) {
    if (!preg_match('/^[0-9\ \+]{0,15}$/', $form_state->getValue(['contact_ph']))) {
      $form_state->setErrorByName('contact_ph', $this->t('Invalid contact phone number'));
    }
    if ($form_state->getValue(['country']) == 'Others') {
      if ($form_state->getValue(['other_country']) == '') {
        $form_state->setErrorByName('other_country', $this->t('Enter country name'));
        // $form_state['values']['country'] = $form_state['values']['other_country'];
      }
      else {
        $form_state->setValue(['country'], $form_state->getValue([
          'other_country'
          ]));
      }
      if ($form_state->getValue(['other_state']) == '') {
        $form_state->setErrorByName('other_state', $this->t('Enter state name'));
        // $form_state['values']['country'] = $form_state['values']['other_country'];
      }
      else {
        $form_state->setValue(['all_state'], $form_state->getValue([
          'other_state'
          ]));
      }
      if ($form_state->getValue(['other_city']) == '') {
        $form_state->setErrorByName('other_city', $this->t('Enter city name'));
        // $form_state['values']['country'] = $form_state['values']['other_country'];
      }
      else {
        $form_state->setValue(['city'], $form_state->getValue(['other_city']));
      }
    }
    else {
      if ($form_state->getValue(['country']) == '') {
        $form_state->setErrorByName('country', $this->t('Select country name'));
        // $form_state['values']['country'] = $form_state['values']['other_country'];
      }
      if ($form_state->getValue(['all_state']) == '') {
        $form_state->setErrorByName('all_state', $this->t('Select state name'));
        // $form_state['values']['country'] = $form_state['values']['other_country'];
      }
      if ($form_state->getValue(['city']) == '') {
        $form_state->setErrorByName('city', $this->t('Select city name'));
        // $form_state['values']['country'] = $form_state['values']['other_country'];
      }
    }
    for ($counter = 1; $counter <= 15; $counter++) {
      $experiment_field_name = 'lab_experiment-' . $counter;
      $experiment_description = 'lab_experiment_description-' . $counter;
      if (strlen(trim($form_state->getValue([$experiment_field_name]))) >= 1) {
        if (strlen(trim($form_state->getValue([$experiment_description]))) <= 49) {
          $form_state->setErrorByName($experiment_description, $this->t('Description should be minimum of 50 characters'));
        }
      }
    }
    if ($form_state->getValue(['version']) == 'olderversion') {
      if ($form_state->getValue(['older']) == '') {
        $form_state->setErrorByName('older', $this->t('Please provide valid version'));
      }
    }

    if (isset($_FILES['files'])) {
      /* check if atleast one source or result file is uploaded */
      // if (!($_FILES['files']['name']['sample_file'])) {
      //   $form_state->setErrorByName('sample_file', $this->t('Please upload zip file.'));
      // }
      /* check for valid filename extensions */
      foreach ($_FILES['files']['name'] as $file_form_name => $file_name) {
        if ($file_name) {
          /* checking file type */
          if (strstr($file_form_name, 'sample')) {
            $file_type = 'S';
          }
          else {
            $file_type = 'U';
          }
          $allowed_extensions_str = '';
          switch ($file_type) {
            case 'S':
              $allowed_extensions_str = \Drupal::config('lab_migration.settings')->
              get('lab_migration_sample_file_extensions');
              break;
          } //$file_type
          $allowed_extensions = explode(',', $allowed_extensions_str);
          $allowd_file = strtolower($_FILES['files']['name'][$file_form_name]);
          $allowd_files = explode('.', $allowd_file);
          $temp_extension = end($allowd_files);
          // if (!in_array($temp_extension, $allowed_extensions)) {
          //   $form_state->setErrorByName($file_form_name, t('Only file with ' . $allowed_extensions_str . ' extensions can be uploaded.'));
          // }
          if ($_FILES['files']['size'][$file_form_name] <= 0) {
            $form_state->setErrorByName($file_form_name, t('File size cannot be zero.'));
          }
          // check if valid file name 
          // if (!lab_migration_check_valid_filename($_FILES['files']['name'][$file_form_name])) {
          //   $form_state->setErrorByName($file_form_name, $this->t('Invalid file name specified. Only alphabets and numbers are allowed as a valid filename.'));
          // }
        } //$file_name
      } //$_FILES['files']['name'] as $file_form_name => $file_name
    } //isset($_FILES['files'])

    return;
  }

  
/**
 * Proposal form submit handler.
 */
public function submitForm(array &$form, FormStateInterface $form_state) {
  $current_user = \Drupal::currentUser();
  /** @var \Drupal\Core\Messenger\MessengerInterface $messenger */
  $messenger = \Drupal::messenger();
  /** @var \Drupal\Core\Database\Connection $database */
  $database = \Drupal::database();
  /** @var \Drupal\Core\Mail\MailManagerInterface $mail_manager */
  $mail_manager = \Drupal::service('plugin.manager.mail');
  /** @var \Drupal\Core\Config\ConfigFactoryInterface $config_factory */
  $config_factory = \Drupal::configFactory();
  /** @var \Drupal\Core\File\FileSystemInterface $file_system */
  $file_system = \Drupal::service('file_system');

  $root_path = \Drupal::service('lab_migration_global')->lab_migration_samplecode_path();

  if (!$current_user->isAuthenticated()) {
    $messenger->addError('It is mandatory to log in to access the proposal form.');
    return;
  }

  $v = $form_state->getValues();
  $solution_provider_uid = ($v['solution_provider_uid'] == "1") ? $current_user->id() : 0;
  $solution_status = ($v['solution_provider_uid'] == "1") ? 1 : 0;

  $solution_provider_name_title = $solution_provider_name = $solution_provider_contact_ph = '';
  $solution_provider_department = $solution_provider_university = '';

  if ($v['solution_provider_uid'] == "1") {
    $solution_provider_name_title = $v['name_title'];
    $solution_provider_name = $v['name'];
    $solution_provider_contact_ph = $v['contact_ph'];
    $solution_provider_department = $v['department'];
    $solution_provider_university = $v['university'];
  }

  $solution_display = 1; // Always 1 in your original.

  // Insert proposal.
  $proposal_id = $database->insert('lab_migration_proposal')
    ->fields([
      'uid' => $current_user->id(),
      'approver_uid' => 0,
      'name_title' => $v['name_title'],
      'name' => $v['name'],
      'contact_ph' => $v['contact_ph'],
      'department' => $v['department'],
      'university' => $v['university'],
      'city' => $v['city'],
      'pincode' => $v['pincode'],
      'state' => $v['all_state'],
      'country' => $v['country'],
      'esim_version' => $v['version'],
      'lab_title' => $v['lab_title'],
      'approval_status' => 0,
      'solution_status' => $solution_status,
      'solution_provider_uid' => $solution_provider_uid,
      'solution_display' => $solution_display,
      'creation_date' => time(),
      'approval_date' => 0,
      'solution_date' => 0,
      'solution_provider_name_title' => $solution_provider_name_title,
      'solution_provider_name' => $solution_provider_name,
      'solution_provider_contact_ph' => $solution_provider_contact_ph,
      'solution_provider_department' => $solution_provider_department,
      'solution_provider_university' => $solution_provider_university,
      'samplefilepath' => '',
    ])
    ->execute();

  if (!$proposal_id) {
    $messenger->addError('Error receiving your proposal. Please try again.');
    return;
  }

  if ($v['solution_provider_uid'] == "1") {
    $dest_path = $proposal_id . '/';
    if (!is_dir($root_path . $dest_path)) {
      $file_system->mkdir($root_path . $dest_path, NULL, TRUE);
    }

    $request = \Drupal::request();
    $files = $request->files->get('files');

    if ($files) {
      foreach ($files as $file_input_name => $uploaded_file) {
        if ($uploaded_file && $uploaded_file->isValid()) {
          $file_name = $uploaded_file->getClientOriginalName();
          $destination = $root_path . $dest_path . $file_name;

          if (file_exists($destination)) {
            $messenger->addError(t("Error uploading file. File %filename already exists.", ['%filename' => $file_name]));
            return;
          }

          $uploaded_file->move($root_path . $dest_path, $file_name);

          $database->update('lab_migration_proposal')
            ->fields(['samplefilepath' => $dest_path . $file_name])
            ->condition('id', $proposal_id)
            ->execute();

          $messenger->addStatus(t('%filename uploaded successfully.', ['%filename' => $file_name]));
        } else {
          $messenger->addError(t('Error uploading file.'));
        }
      }
    }
  }

  // Insert experiments.
  $number = 1;
  for ($counter = 1; $counter <= 15; $counter++) {
    $experiment_field_name = 'lab_experiment-' . $counter;
    $experiment_description = 'lab_experiment_description-' . $counter;

    $title = trim($v[$experiment_field_name]);
    $description = trim($v[$experiment_description]);

    if (strlen($title) >= 1) {
      $database->insert('lab_migration_experiment')
        ->fields([
          'proposal_id' => $proposal_id,
          'number' => $number,
          'title' => $title,
          'description' => $description,
        ])
        ->execute();
      $number++;
    }
  }

  // Send email.
  $config = $config_factory->get('lab_migration.settings');
  $from = $config->get('from_email');
  $bcc = $config->get('emails');
  $cc = $config->get('cc_emails');

  $params = [];
  $params['proposal_received']['proposal_id'] = $proposal_id;
  $params['proposal_received']['user_id'] = $current_user->id();
  $params['proposal_received']['headers'] = [
    'From' => $from,
    'MIME-Version' => '1.0',
    'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
    'Content-Transfer-Encoding' => '8Bit',
    'X-Mailer' => 'Drupal',
    'Cc' => $cc,
    'Bcc' => $bcc,
  ];

  $result = $mail_manager->mail('lab_migration', 'proposal_received', $current_user->getEmail(), $current_user->getPreferredLangcode(), $params, $from, TRUE);
  if ($result['result'] !== TRUE) {
    $messenger->addError('Error sending email message.');
  }

  $messenger->addStatus(t('We have received your Lab migration proposal. We will get back to you soon.'));

  // Redirect to front page.
  // $response = new RedirectResponse(\Drupal::url('<front>'));
  // $response->send();
}
}
?>