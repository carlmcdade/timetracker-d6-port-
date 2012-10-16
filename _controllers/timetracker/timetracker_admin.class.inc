<?php

/*
 * @author Carl McDade
 * @copyright Copyright (c) 2010, Carl McDade
 * @since 2010-02-19
 * @version 0.2
 * @license
 * @link 
 *
 */


class timetracker_admin{
	
	private $common;
	public  $page_title = 'Time Tracker Administration';
	public  $dbc;
	


    function __construct()
    {
        //Test the Common settings
        $this->common = Common::get_instance();
        $this->section = get_class($this);
    }

    function admin_links()
    {
        $section = get_class($this);
    	$var[ $section .'_1'] = array(
        		'text' => 'Entries', 
        		'path' => 'admin/timetracker/reports'
        		);
        
        $var[$section .'_2'] = array(
        		'text' => 'Projects', 
        		'path' => 'admin/timetracker/projects'
        		);
        
        $var[$section .'_3'] = array(
        		'text' => 'Groups', 
        		'path' => 'admin/timetracker/groups'
        		);
        

        return $var;
    }
    
    function index()
    {
    	
    	// admin links
        $variables['links'] = _menu_links($this->admin_links(),' | ');
    	//
    	$variables['content'] = 'administration index';
    	return _view('default', $variables);  
    }
    
    
    function settings()
    {   	
    	// admin links
        
        print variable_get('timetracker_projects', 0);
        $output = drupal_get_form('timetracker_projects_category_add_');
        $output .= drupal_get_form('timetracker_settings_form_');
    	$variables['content'] = $output;
        return _view('default', $variables);  	  	
    
    }
	
	function settings_save()
	{
		//
		variable_set('timetracker_projects', $_POST['selected']);
		drupal_set_message(t('The timetracker category is has been set to "'). variable_get('timetracker_projects', 'projects') . '".');
		drupal_goto('timetracker/admin/timetracker/settings');
	}
	
	function reports($extend = NULL)
    {
        
		// title for content
		$variables['title'] = t('Administer Reports');
			
		// admin links
		$variables['links'] = _menu_links($this->admin_links(),' | ');
			
    	$table_header = array(
					array('data' => 'ID', 'field' => 'id', 'sort' => 'asc'),
					array('data' => 'Task', 'field' => 'project_user_task'),
					array('data' => 'Name', 'field' => 'project_name'),
					array('data' => 'User', 'field' => 'user_id'),
					array('data' => 'Hours', 'field' => 'project_hours'),
					array('data' => 'Event', 'field' => 'event_date'),
					array('data' => 'Created', 'field' => 'created'),
					array('data' => 'Operations', '' => '', 'colspan' => 2),
					
					);
		
		$query = "SELECT id, project_user_task,project_name,user_id,project_hours,event_date,created FROM {timetracker_project_events} as tt";
		$query .= tablesort_sql($table_header);
		
		$limit = 10;
		
		//generate a paged query
		$result = pager_query($query, $limit);
		
		// Result is returned as a iterable object that returns a stdClass object on each iteration
		if($result->num_rows > 0)
		{
			
			//set up data array
			$data = '';
			$i = 1;
			//your basic while loop to get the data
			while($record = db_fetch_object($result))
			{				  
					  $row_data[] = array(
							  'data' => array(
								  $record->id,
								  $record->project_user_task, 
								  $record->project_name,
								  $record->user_id,
								  $record->project_hours,
								  $record->event_date,
								  $record->created,
								  l(t('delete'), '', array('attributes' => array('class' => array('delete-time'), 'id' => 'del-' . $record->id))),
								  l(t('update'), APP_MODULE .'/timetracker/update_time/'.$record->id, array('attributes' => array('class' => array('update-time'), 'id' => 'upd-' . $record->id))) 
					  
								  ),
							  'id' => array('id' => 'row-'.$record->id),
							  );
			}
			
			
			// Format and print out the table.
			$output = theme('table',$table_header, $row_data);
			
			$output .= theme('pager', NULL, $limit, 0);
				
			// monthly totals for this user by project
			$variables['content'] = $output;
	
			return _view('default', $variables);
			
		}
		else
		{
			
			//
			$variables['content'] = t('No entries have been made. ');    
			return _view('default', $variables);
			
		}
		
    }
    
    /**
    * @author Carl McDade
    * @name project administration
    * Programmatically add and delete taxonomy terms 
    * separately from the default taxonomy admin 
    * so that Managers do not get full backend previleges
    *
    */
    
    function projects($extend = NULL)
    {
        // title for content
    	$variables['title'] = t('');
    	
    	// admin links
        $variables['links'] = _menu_links($this->admin_links(),' | ');       
        
    	// get vocabulary as options list
    	
		$voc = variable_get('timetracker_projects', NULL);
		if(!$voc)
		{
			//
			$variables['content'] = t('No vocabulary has been assigned ' . l('set project category', 'admin/timetracker/settings'));
       
			return _view('default', $variables);
		}
		$tree = taxonomy_get_tree($voc,0,NULL,true);
		
		$table_header = array(
				array('data' => 'ID', 'field' => 'tid', 'sort' => 'asc'),
				array('data' => 'Group', 'field' => 'project_group_id'),
				array('data' => 'Name', 'field' => 'project_name'),
				array('data' => 'Code', 'field' => 'project_external_key'),
				array('data' => 'Description', 'field' => 'description'),
				array('data' => 'Hours', 'field' => 'project_hours'),
				array('data' => 'Created', 'field' => 'created'),
				array('data' => 'Status', 'field' => 'status_name'),
				array('data' => 'Operations', '' => '', 'colspan' => 2),
				
				);
		
		$query = "SELECT td.tid, td.name, td.description, tt.project_external_key, tt.created, tt.project_hours, g.project_group_name, g.project_group_prefix, s.* FROM 
		{term_data} AS td 
		LEFT JOIN {timetracker_projects} AS tt ON td.tid = tt.project_id 
		LEFT JOIN {timetracker_project_groups} AS g ON g.project_group_id = tt.project_group_id 
		LEFT JOIN {timetracker_project_status} AS s ON tt.status = s.id WHERE td.vid =" . $voc;
		
		$query .= tablesort_sql($table_header);
		
		$limit = 10;
		
		//generate a paged query
		$result = pager_query($query, $limit);
			
		// Result is returned as a iterable object that returns a stdClass object on each iteration
		if($result){
			//set up data array
			$data = '';
			$i = 1;
			//your basic while loop to get the data
			while($record = db_fetch_object($result))
			{				
			
				/* get the total number of hours for each project */
				$hours_total = 0;
				
				$hours = db_query("SELECT e.project_hours FROM {timetracker_project_events} AS e WHERE project_id =" . $record->tid);
					
						
				while($event = db_fetch_object($hours))
				{
					$hours_total += $event->project_hours;
				}
				
				//$output .= print_r($record,1);
				$row_data[] = array(
					'data' => array(
						$record->tid,
						$record->project_group_name, 
						$record->name,
						$record->project_group_prefix . '-' . $record->project_external_key,
						$record->description,
						$hours_total,
						$record->created,
						$record->status_name,
						l(t('edit'), 'admin/'. APP_MODULE .'/project_edit/'.$record->tid, array('attributes' => array('class' => array('update-time'), 'id' => 'upd-' . $record->tid))) 
						),
						'id' => 'row-'.$record->tid,
						);
			
			}
		}
		
		// show projects add form 
        $output = drupal_get_form('timetracker_projects_add_');
        
        $output .= '<div id="project-list">' . t('A list of available projects and their descriptions:') . '</div>';
    	
		
		// Format and print out the table.
		$output .= theme('table',$table_header, $row_data);
		
		$output .= theme('pager', NULL, $limit, 0);
               
    	$variables['content'] = $output;
       
        return _view('default', $variables);
    }
    
    function project_edit()
    {
    	// title for content
    	$variables['title'] = t('');

    	
    	// admin links
        $variables['links'] = _menu_links($this->admin_links(),' | ');
        
    	$output = drupal_get_form('timetracker_projects_edit_');
    	$variables['content'] = $output;
        
        return _view('default', $variables);
    }
    
    function project_save($tid=NULL)
    {
    	/* we may need to know who by and when a project was created */
    	global $user;
    	
    	$voc = variable_get('timetracker_projects', NULL);			

    	$term = new stdClass();
		$term->name = check_plain($_POST['project_name']);
		$term->description = check_plain($_POST['project_desc']);
		$term->vid = $voc;
		
		/* save to the Drupal taxonomy system which creates a term/project */
		taxonomy_term_save($term);		
		
		print t('project taxonomy created'); 
		
		/* get the term and information that was inserted into the database */
		$new = $term->tid;
    			
    	$term_info = taxonomy_term_load($new);
    	
    	$data = array(
				  'project_id' => $new,
				  'project_name' => $term_info->name,
				  'project_desc' => $term_info->description,
				  'project_external_key' => check_plain($_POST['project_external_key']),// since the terms do not have fields we add fk an external table and hold the extra information there
				  'created' => date("Y-m-d H:i:s"),
				  'changed' => date("Y-m-d H:i:s"),
				  'vid' => $voc
				);
		
		drupal_write_record('timetracker_projects', $data);
				
		print t('project added to timetracker'); 
    }
    
    function project_update()
    {
    	/* we may need to know who by and when a project was created */
    	global $user;
    	
    	$voc = variable_get('timetracker_projects', NULL);			
    	
    	
    	// update the taxonomy table
    	$term = new stdClass();
    	$term->tid = check_plain($_POST['project_id']);
		$term->name = check_plain($_POST['project_name']);
		$term->description = check_plain($_POST['project_desc']);
		$term->vid = $voc->vid;

		
		/* save to the Drupal taxonomy system which creates a term/project */
		taxonomy_term_save($term);

						
		$project_id = db_fetch_object(db_query("SELECT * FROM {timetracker_projects} AS tp WHERE project_id=%d", array($term->tid)));						
		
    	// update the timetracker project table
    	if(is_object($project_id))
    	{
			$project_info = db_query("UPDATE {timetracker_projects} 
				SET (
				project_name =" . $term->name . "," . 
				"project_desc =" . $term->description  . "," . 
				"project_group_id =" . check_plain($_POST['project_group']) . "," . 
				"project_external_key =" . check_plain($_POST['project_external_key']) . ","  .
				"changed =" . date("Y-m-d H:i:s") . "," . 
				"status =" . check_plain($_POST['project_status']) . ")" .
				" WHERE project_id= %d", array($term->tid));
					
			drupal_set_message( t('project updated'));
					
		}
		else
		{		 
					
			$project_info = db_query("INSERT INTO {timetracker_projects} 
				VALUES(project_id,project_group_id, project_name,project_desc,project_external_key,created,changed,status, vid 
				SET(" . $term->name .",". 
				$term->description . "," . 
				check_plain($_POST['project_group']) . "," .  
				check_plain($_POST['project_external_key']) . "," . 
				date("Y-m-d H:i:s") . "," .
				date("Y-m-d H:i:s") . "," .
				check_plain($_POST['project_status']) . "," .
				$voc );
					
			drupal_set_message( t('project and taxonomy tables syncronised.'));
				
		}		
		
		 
		drupal_goto('admin/timetracker/project_edit/'. $term->tid);
		
    }
    
    function project_category_save()
    {
    	//    	
		$project = check_plain($_POST['project_category_name']);
		$description = check_plain($_POST['project_category_description']); 
		$machine_name = check_plain($_POST['project_category_machine_name']);
    	
    	$vocabulary = array(
				'name' => $project, //Human readable name of the vocabulary
				'module' => $machine_name,
				'description' => $description,
				'multiple' => 0, //set 1 to allow multiple selection
				'required' => 0, //set 1 to make the terms mandatory to be selected
				'hierarchy' => 0, //set 1 to allow and create hierarchy of the terms within the vocabulary
				'relations' => 0, //set 1 to set and allow relation amongst multiple terms
				'weight' => -9, //set the weight to display the vocabulary in the list
				);

		$voc = taxonomy_save_vocabulary($vocabulary); 
		
		variable_set('timetracker_projects', $voc ,0);	
				
		drupal_set_message(t('Project category added and set  to timetracker list'));
		
		drupal_goto('admin/timetracker/settings');
		
    }
    
    function groups($extend = NULL)
    {
        // title for content
    	$variables['title'] = t('');
  	
    	// admin links
        $variables['links'] = _menu_links($this->admin_links(),' | ');
                
    	// get vocabulary as options list
		$voc = variable_get('timetracker_projects', 0);
		$tree = taxonomy_get_tree($voc,0,NULL,true);
		
		$table_header = array(
				array('data' => 'ID', 'field' => 'project_group_id', 'sort' => 'asc'),
				array('data' => 'Name', 'field' => 'project_group_name'),
				array('data' => 'Prefix Code', 'field' => 'project_group_prefix'),
				array('data' => 'Description', 'field' => 'project_group_description'),
				
				);
		
		$query = "SELECT gr.project_group_id, gr.project_group_name, gr.project_group_description, gr.project_group_prefix FROM 
		{timetracker_project_groups} AS gr";
		
		$query .= tablesort_sql($table_header);
		
		$limit = 10;
		
		//generate a paged query
		$result = pager_query($query, $limit);
			
		// Result is returned as a iterable object that returns a stdClass object on each iteration
		if($result->num_rows > 0){
			//set up data array
			$data = '';
			$i = 1;
			//your basic while loop to get the data
			while($record = db_fetch_object($result))
			{				
			
				//$output .= print_r($record,1);
				$row_data[] = array(
					'data' => array(
						$record->project_group_id,
						$record->project_group_name, 
						$record->project_group_prefix,
						$record->description,
						l(t('edit'), 'admin/'. APP_MODULE .'/group_edit/'.$record->tid, array('attributes' => array('class' => array('update-time'), 'id' => 'upd-' . $record->tid))) 
						),
						'id' => 'row-'.$record->project_group_id,
						);
			
			}
			// show projects add form 
			$output = drupal_get_form('timetracker_groups_add_');
			
			$output .= '<div id="project-list">' . t('A list of available groups and their descriptions:') . '</div>';
			
			
			// Format and print out the table.
			$output .= theme('table',$table_header, $row_data);
			
			$output .= theme('pager', NULL, $limit, 0);
				   
			$variables['content'] = $output;
		   
			return _view('default', $variables);
		}
		else
		{
			
			//
			$output = drupal_get_form('timetracker_groups_add_');
			$output .= t('No group entries have been made. ');
			$variables['content'] =  $output;
			
			return _view('default', $variables);
			
		}
		
		
    }
    
    
}

?>
